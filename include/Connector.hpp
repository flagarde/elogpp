#pragma once

#include <string>

typedef struct ssl_st SSL;

namespace elogpp
{

  class Connector
  {
  public:
    Connector();
    void setHostname(const std::string& hostname);
    std::string getHostname();
    void setPort(const unsigned int& port);
    unsigned int getPort();
    void setVerbosity(bool verbose);
    bool getVerbosity();
    void setSSL(bool ssl);
    bool getSSL();
    void connect();
    void disconnect();
    ~Connector();
    std::string receive();
    void send(char request[100000],std::size_t length=0);
    void send(const std::string& request);
  private:
    int ssl_connect();
    std::string m_Hostname{""};
    unsigned int m_Port{80};
    bool m_SSL{false};
    bool m_Verbose{false};
    SSL* m_SSL_con{nullptr};
    int m_Sock{0};
  };
  
}
