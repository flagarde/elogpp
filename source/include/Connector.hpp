#pragma once

#include <string>

typedef struct ssl_st SSL;

namespace elogpp
{

class Connector
{
public:
  Connector();
  void setVerbose(const bool& verbose);
  bool getVerbose();
  void setHostname(const std::string& hostname);
  std::string getHostname();
  void setPort(const unsigned int& port);
  unsigned int getPort();
  void setSSL(const bool& ssl);
  bool getSSL();
  int connect();
  void sendRequest(const std::string& request,std::size_t length=0);
  std::string receiveRespond();
  void disconnect();
  ~Connector();
private:
  
  int ssl_connect();
  std::string m_Hostname{""};
  unsigned int m_Port{80};
  bool m_Verbose{false};
  int m_Sock{0};
  SSL* m_SSL_con{nullptr};
  bool m_SSL{false};
};
  
}
