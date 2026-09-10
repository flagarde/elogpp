#pragma once

#include <string>
#include <string_view>
#include <memory>
#include <filesystem>

typedef struct ssl_st SSL;

namespace cxx::elog
{

  class Connector
  {
  public:
    Connector();
    ~Connector() noexcept;
    Connector(Connector&& other) noexcept;
    Connector& operator=(Connector&& other) noexcept;
    Connector(const Connector&) = delete;
    Connector& operator=(const Connector&) = delete;

    void setHostname(const std::string_view hostname) { m_Hostname=hostname; }
    std::string_view getHostname() const noexcept { return m_Hostname; }
    void setPort(const std::uint16_t port) { m_Port = port; }
    std::uint16_t getPort() const noexcept { return m_Port; }
    void setVerbosity(const bool verbose) noexcept { m_Verbose = verbose; }
    bool getVerbosity() const noexcept { return m_Verbose; }
    void setSSL(const bool ssl);
    bool getSSL() const noexcept { return m_SSL; }
    void connect();
    void disconnect();
    std::string receive();
    void send(const std::string_view request);
    void setCACertificateFile(const std::filesystem::path& path) { s_CACertFilePath = path; }
    std::filesystem::path getCACertificateFile() const noexcept { return s_CACertFilePath; }
    void setCACertificatePath(const std::filesystem::path& path) { s_CACertDirPath = path; }
    std::filesystem::path getCACertificatePath() const noexcept { return s_CACertDirPath; }

  private:
    struct SSLDeleter
    {
      void operator()(SSL* ssl) const;
    };
    int ssl_connect();
    std::string m_Hostname;
    std::uint16_t m_Port{80};
    bool m_SSL{false};
    bool m_Verbose{false};
    std::unique_ptr<SSL,SSLDeleter> m_SSL_con{nullptr};
    int m_Sock{0};
    std::filesystem::path s_CACertFilePath;
    std::filesystem::path s_CACertDirPath;
  };

}
