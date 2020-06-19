#include "Connector.hpp"

/* SSL includes */
#ifdef HAVE_SSL
  #include <openssl/ssl.h>
#endif

#include <iostream>
#include <cstring>

#ifdef _MSC_VER
  #include <windows.h>
  #include <io.h>
#else
  #include <netdb.h>
  #include <netinet/in.h>
  #include <sys/socket.h>
  #include <sys/time.h>
  #include <unistd.h>
  #include <signal.h>
  #define closesocket(s) ::close(s)
#endif


namespace elogpp
{

Connector::Connector()
{
  #if defined( _MSC_VER )
  {
    WSADATA WSAData;
    if (WSAStartup(MAKEWORD(1, 1), &WSAData) != 0) return -1;
  }
  #endif
}
  
void Connector::setVerbose(const bool& verbose)
{
  m_Verbose=verbose;
}

bool Connector::getVerbose()
{
  return m_Verbose;
}
  
void Connector::setHostname(const std::string& hostname)
{
  m_Hostname=hostname;
}

std::string Connector::getHostname()
{
  return m_Hostname;
}
  
void Connector::setPort(const unsigned int& port)
{
  m_Port=port;
}
  
unsigned int Connector::getPort()
{
  return m_Port;
}
  
void Connector::setSSL(const bool& ssl)
{
  #ifndef HAVE_SSL
  m_SSL=ssl;
  #else
  if(ssl)
  {
    std::cout<<"SLL support not compiled into this program\n";
    throw 1;
  }
  #endif
}
  
bool Connector::getSSL()
{
  return m_SSL;
}
  
int Connector::connect()
{
  if(m_Hostname.empty())
  {
    std::cout<<"Please specify hostname.\n";
    return -1;
  }
  if((m_Sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
  {
    std::cout<<"cannot create socket\n";
    return -1;
  }
  struct sockaddr_in bind_addr;
  bind_addr.sin_family = AF_INET;
  bind_addr.sin_addr.s_addr = 0;
  bind_addr.sin_port = htons((unsigned short) m_Port);
    
  struct hostent* phe = gethostbyname(m_Hostname.c_str());
  if(phe == nullptr) 
  {
    std::cout<<"cannot get host name\n";
    return -1;
  }
  memcpy((char *) &(bind_addr.sin_addr), phe->h_addr, phe->h_length);
  int status = ::connect(m_Sock, (const sockaddr*) &bind_addr, sizeof(bind_addr));
  if (status != 0) 
  {
    std::cout<<"Cannot connect to host "<<m_Hostname<<", port "<<m_Port<<"\n";
    return -1;
  }
  if(m_Verbose) std::cout<<"Successfully connected to host "<<m_Hostname<<", port "<<m_Port<<"\n";
  #ifdef HAVE_SSL
  if(m_SSL && ssl_connect() < 0) 
  {
    std::cout<<"elogd server does not run SSL protocol\n";
    return -1;
  }
  #endif
  return 0;
}
  
int Connector::ssl_connect()
{
  #ifdef HAVE_SSL
  SSL_library_init();
  SSL_load_error_strings();
  #if OPENSSL_VERSION_NUMBER > 0x1010000fL
  SSL_METHOD* meth{(SSL_METHOD *) TLS_method()};
  #else
  SSL_METHOD* meth{(SSL_METHOD *) TLSv1_2_method()};
  #endif
  SSL_CTX* ctx{SSL_CTX_new(meth)};
  m_SSL_con={SSL_new(ctx)};
  SSL_set_fd(m_SSL_con, m_Sock);
  if(SSL_connect(m_SSL_con) <= 0) return -1;
  X509* cert{SSL_get_peer_certificate(m_SSL_con)};
  if(cert == nullptr) return -1;
  long int i{SSL_get_verify_result(m_SSL_con)};
  if(i != X509_V_OK) std::cout<<"Possibly invalid certificate, continue on your own risk!\n";
  #endif
  return 0;
}
  
void Connector::sendRequest(const std::string& request,std::size_t length)
{
  if(length==0) length=strlen(request.c_str());
  #ifdef HAVE_SSL
  if(m_SSL)
    SSL_write(m_SSL_con,request.c_str(),length);
  else
    #endif
    send(m_Sock,request.c_str(),length,0);
  if(m_Verbose) 
  {
    std::cout<<"Request sent to host:\n";
    puts(request.c_str());
  }
}
  
std::string Connector::receiveRespond()
{
  char response[100000];
  int i{0};
  #ifdef HAVE_SSL
  if(m_SSL)
    i=SSL_read(m_SSL_con,response, sizeof(response) - 1);
  else
  #endif
    i=recv(m_Sock, response, sizeof(response) - 1, 0);
  if(i < 0)
  {
    std::cout<<"Cannot receive response"<<std::endl;
    throw -1;
  }
  int n{i};
  while (i > 0) 
  {
    #ifdef HAVE_SSL
    if(m_SSL)
      i = SSL_read(m_SSL_con, response + n, sizeof(response) - 1 - n);
    else
    #endif
      i = recv(m_Sock, response + n, sizeof(response) - 1 - n, 0);
    if(i > 0)n += i;
  }
  response[n] = 0;
  if(m_Verbose)
  {
    std::cout<<"Response received:\n";
    puts(response);
  }
  return std::string(response);
}
  
void Connector::disconnect()
{
  #ifdef HAVE_SSL
  if(m_SSL) 
  {
    SSL_shutdown(m_SSL_con);
    SSL_free(m_SSL_con);
  }
  #endif
  closesocket(m_Sock);
}

Connector::~Connector()
{
  disconnect();
}
  
}
