#include "Connector.hpp"

#include <algorithm>
#include <string_view>

/* SSL includes */
#if defined(HAVE_SSL)
  #include <openssl/ssl.h>
  #include <memory>
  #include <stdexcept>
  using SSL_CTX_ptr = std::unique_ptr<SSL_CTX, decltype(&::SSL_CTX_free)>;
  static const int g_ssl_error_idx = SSL_get_ex_new_index(0, (void*)("ssl_error_idx"), nullptr, nullptr, nullptr);
#endif


#include <iostream>
#include <cstring>

#ifdef _WIN32
  #include <windows.h>
  #include <io.h>
#else
  #include <netdb.h>
  #include <unistd.h>
  #define closesocket(s) ::close(s)
  #ifndef O_BINARY
    #define O_BINARY 0
  #endif
#endif

namespace cxx::elog
{

  Connector::~Connector() noexcept
  {
    disconnect();
  }
  Connector::Connector(Connector&& other) noexcept = default;
  Connector& Connector::operator=(Connector&& other) noexcept = default;

  void Connector::SSLDeleter::operator()(::SSL* ssl) const
  {
    if(ssl) ::SSL_free(ssl);
  }

  Connector::Connector()
  {
    #if defined(_WIN32)
    {
      WSADATA WSAData;
      if(WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) throw -1;
    }
    #endif
  }

  void Connector::setSSL(const bool ssl)
  {
    #ifndef HAVE_SSL
    if(ssl)
    {
      std::cout<<"SLL support not compiled into this program\n";
    }
    #else
    m_SSL=ssl;
    #endif
  }

  void Connector::connect()
  {
    if(m_Hostname.empty())
    {
      std::cout<<"Please specify hostname.\n";
      throw -1;
    }
    if((m_Sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
      std::cout<<"cannot create socket\n";
      throw -1;
    }
    struct sockaddr_in bind_addr;
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = 0;
    bind_addr.sin_port = htons((unsigned short) m_Port);

    struct hostent* phe = gethostbyname(m_Hostname.c_str());
    if(phe == nullptr)
    {
      std::cout<<"cannot get host name\n";
      throw -1;
    }
    memcpy((char *) &(bind_addr.sin_addr), phe->h_addr, phe->h_length);
    int status = ::connect(m_Sock, (const sockaddr*) &bind_addr, sizeof(bind_addr));
    if (status != 0)
    {
      std::cout<<"Cannot connect to host "<<m_Hostname<<", port "<<m_Port<<"\n";
      throw -1;
    }
    if(m_Verbose) std::cout<<"Successfully connected to host "<<m_Hostname<<", port "<<m_Port<<"\n";
    ssl_connect();
  }
  void Connector::disconnect()
  {
    #ifdef HAVE_SSL
    if(m_SSL)
    {
      SSL_shutdown(m_SSL_con.get());
    }
    #endif
    closesocket(m_Sock);
  }

  std::string Connector::receive()
  {
    constexpr std::size_t chunk_size{2048};
    thread_local std::string data;
    data.clear();
    if(data.capacity() < chunk_size) data.reserve(chunk_size);

    std::size_t header_end = std::string::npos;
    while(header_end == std::string::npos)
    {
      std::size_t old_size = data.size();
      data.resize(old_size + chunk_size);

        int n;
#ifdef HAVE_SSL
        if (m_SSL)
            n = SSL_read(m_SSL_con.get(), &data[old_size], chunk_size);
        else
#endif
            n = recv(m_Sock, &data[old_size], chunk_size, 0);

        if (n > 0)
        {
            data.resize(old_size + n);

            if (data.size() > 64 * 1024)
                throw std::runtime_error("HTTP headers too large");

            header_end = data.find("\r\n\r\n");
        }
        else
        {
            data.resize(old_size);
            if (n == 0)
                throw std::runtime_error("Connection closed before HTTP headers");
            else
                throw std::runtime_error("HTTP receive error");
        }
    }

    // --- CORRECTION : RECHERCHE DU CONTENT-LENGTH INSENSIBLE À LA CASSE ---
    // On isole les en-têtes uniquement pour l'analyse
    std::string headers_lower = data.substr(0, header_end);
    std::transform(headers_lower.begin(), headers_lower.end(), headers_lower.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    const std::string_view target = "content-length:";
    size_t pos = headers_lower.find(target);

    if (pos == std::string::npos) {
        // Pas de Content-Length trouvé (Chunked, pas de body, ou connexion close)
        return data;
    }

    // On récupère la valeur depuis la chaîne d'origine 'data' à l'index correspondant
    pos += target.length();
    size_t end_pos = data.find_first_of("\r\n", pos);
    if (end_pos == std::string::npos) {
        throw std::runtime_error("En-tête HTTP malformé après Content-Length");
    }

    std::string value_str = data.substr(pos, end_pos - pos);

    // Nettoyage des espaces blancs
    value_str.erase(0, value_str.find_first_not_of(" \t"));
    value_str.erase(value_str.find_last_not_of(" \t") + 1);

    if (value_str.empty()) {
        throw std::runtime_error("En-tête Content-Length vide ou malformé");
    }

    std::size_t content_length = std::stoul(value_str);

    size_t body_start_pos = header_end + 4;
    size_t bytes_already_read = data.size() - body_start_pos;

    // --- BOUCLE 2 : LECTURE DU RESTE DU CORPS ---
    size_t bytes_to_read = 0;
    if (content_length > bytes_already_read) {
        bytes_to_read = content_length - bytes_already_read;
    }

    if (bytes_to_read > 0) {
        size_t current_size = data.size();
        data.resize(current_size + bytes_to_read);

        size_t total_bytes_read_in_loop = 0;
        while (total_bytes_read_in_loop < bytes_to_read) {
            char* write_ptr = &data[current_size + total_bytes_read_in_loop];
            size_t max_to_read = bytes_to_read - total_bytes_read_in_loop;

            int n;
#ifdef HAVE_SSL
            if (m_SSL)
                n = SSL_read(m_SSL_con.get(), write_ptr, max_to_read);
            else
#endif
                n = recv(m_Sock, write_ptr, max_to_read, 0);

            if (n > 0) {
                total_bytes_read_in_loop += n;
            }
            else if (n == 0) {
                throw std::runtime_error("Connexion fermée par le serveur avant d'avoir reçu tout le corps HTTP");
            }
            else {
                throw std::runtime_error("Erreur de lecture du corps HTTP");
            }
        }
    }

    //std::cout << "Données totales reçues avec succès !" << std::endl;

    if (data.capacity() > 1024 * 1024) {
        data.shrink_to_fit();
    }

    std::string res;
    std::swap(res, data);

    return res;




  }

  void Connector::send(const std::string_view request)
  {
    #ifdef HAVE_SSL
    if(m_SSL)
      SSL_write(m_SSL_con.get(),&request[0],request.size());
    else
      #endif
      ::send(m_Sock,&request[0],request.size(),0);
    if(m_Verbose)
    {
      std::cout<<"Request sent to host:\n"<<request<<std::endl;
    }
  }


  int Connector::ssl_connect()
  {
    #if defined(HAVE_SSL)
      struct ConnectionError
      {
        std::string message;
        bool has_error{false};
      };

      const SSL_CTX_ptr m_SSL_ctx
      {
        [this]()
        {
          SSL_CTX_ptr ctx(SSL_CTX_new(TLS_method()),&SSL_CTX_free);
          if(!ctx) throw std::runtime_error("SSL_CTX_new(TLS_method()) failed!");

          const bool has_file = !s_CACertFilePath.empty() && std::filesystem::exists(s_CACertFilePath);
          const bool has_path = !s_CACertDirPath.empty() && std::filesystem::is_directory(s_CACertDirPath);

          if(has_file || has_path)
          {
            std::string file_str = has_file ? s_CACertFilePath.string() : "";
            std::string path_str = has_path ? s_CACertDirPath.string() : "";

            const char* file_ptr = has_file ? file_str.c_str() : nullptr;
            const char* path_ptr = has_path ? path_str.c_str() : nullptr;

            if(SSL_CTX_load_verify_locations(ctx.get(), file_ptr, path_ptr) != 1) throw std::runtime_error("SSL_CTX_load_verify_locations failed for configured paths.");
          }
          else
          {
            if(SSL_CTX_set_default_verify_paths(ctx.get()) != 1) std::cout << "Warning: Could not load default system certificates.\n";
          }
          SSL_CTX_set_verify
          (
            ctx.get(),
            SSL_VERIFY_PEER,
            [](int preverify_ok, X509_STORE_CTX* x509_ctx) -> int
            {
              if (preverify_ok) return 1;
              const SSL* ssl = static_cast<SSL*>(X509_STORE_CTX_get_ex_data(x509_ctx, SSL_get_ex_data_X509_STORE_CTX_idx()));
              if(ssl)
              {
                ConnectionError* err_state = static_cast<ConnectionError*>(SSL_get_ex_data(ssl, g_ssl_error_idx));
                if(err_state)
                {
                  const int err = X509_STORE_CTX_get_error(x509_ctx);
                  err_state->has_error = true;
                  err_state->message = std::string("SSL Warning intercepted during handshake: ") + std::string(X509_verify_cert_error_string(err)) + std::string(" (Code: ") + std::to_string(err) + std::string(")");
                }
              }
              return 1;
            }
          );
          return std::move(ctx);
        }
        ()
      };

      if(!m_SSL_ctx) throw std::runtime_error("SSL_CTX_new(TLS_method()) failed!");
      m_SSL_con.reset(SSL_new(m_SSL_ctx.get()));
      if(!m_SSL_con) throw std::runtime_error("SSL_new(m_SSL_ctx.get()) failed!");
      ConnectionError conn_error;
      SSL_set_ex_data(m_SSL_con.get(), g_ssl_error_idx, &conn_error);
      if(SSL_set_fd(m_SSL_con.get(), m_Sock)==0) throw std::runtime_error("SSL_set_fd(m_SSL_con.get(), m_Sock) failed");

      /*SSL_set_info_callback
      (
        m_SSL_con.get(),
        [](const SSL* ssl, int type, int val)
        {
          if(type & SSL_CB_LOOP)
          {
            std::cout << "[SSL Debug] Loop: " << SSL_state_string_long(ssl) << "\n";
          }
          else if(type & SSL_CB_ALERT)
          {
            std::string direction = (type & SSL_CB_READ) ? "Read" : "Write";
            std::cout << "[SSL Debug] Alert (" << direction << "): "<< SSL_alert_type_string_long(val) << " : "<< SSL_alert_desc_string_long(val) << "\n";
          }
          else if(type & SSL_CB_HANDSHAKE_START)
          {
            std::cout << "[SSL Debug] Handshake started\n";
          }
          else if(type & SSL_CB_HANDSHAKE_DONE)
          {
            std::cout << "[SSL Debug] Handshake completed successfully using: "<< SSL_get_cipher(ssl) << " (" << SSL_get_version(ssl) << ")\n";
          }
        }
      );*/
      if(SSL_set1_host(m_SSL_con.get(), m_Hostname.c_str()) != 1) throw std::runtime_error("SSL_set1_host failed for: " + m_Hostname);
      if(SSL_set_tlsext_host_name(m_SSL_con.get(), m_Hostname.c_str()) != 1) throw std::runtime_error("SSL_set_tlsext_host_name (SNI) failed for: " + m_Hostname);

      if(SSL_connect(m_SSL_con.get()) <= 0)
      {
        if(conn_error.has_error) throw std::runtime_error(conn_error.message);
        throw std::runtime_error("Handshake failed due to an unknown network/protocol error.");
      }

      const std::unique_ptr<X509, decltype(&::X509_free)> cert{SSL_get1_peer_certificate(m_SSL_con.get()), &::X509_free};
      if(cert == nullptr) throw std::runtime_error("SSL_get1_peer_certificate(m_SSL_con.get()) failed");


      const long int i{SSL_get_verify_result(m_SSL_con.get())};
      if(i != X509_V_OK) throw std::runtime_error("SSL Verification Error (" + std::to_string(i) + "): " + X509_verify_cert_error_string(i));
    #endif
    return 0;
  }

}