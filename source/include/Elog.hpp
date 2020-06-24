#pragma once
#include "Connector.hpp"
#include "Crypt.hpp"

#include<vector>
#include<map>
#include<string>

namespace elogpp
{

enum Type { Edit, Download, Reply, New };

class Elog
{
public:
  void setType(const Type& type)
  {
    m_Type=type;
  }
  Type getType()
  {
    return m_Type;
  }
  void setVerbosity(const bool& verbose)
  {
    m_Connector.setVerbosity(verbose);
  }
  bool getVerbosity()
  {
    return m_Connector.getVerbosity();
  }
  void setSSL(const bool& ssl)
  {
    m_Connector.setSSL(ssl);
  }
  bool getSSL()
  {
    return m_Connector.getSSL();
  }
  void setHostname(const std::string& hostname)
  {
    m_Connector.setHostname(hostname);
  }
  std::string getHostname()
  {
    return m_Connector.getHostname();
  }
  void setPort(const unsigned int& port)
  {
    m_Connector.setPort(port);
  }
  unsigned int getPort()
  {
    return m_Connector.getPort();
  }
  void connect()
  {
    m_Connector.connect();
  }
  void disconnect()
  {
    m_Connector.disconnect();
  }
  void setID(const int& ID)
  {
    m_ID=ID;
  }
  int getID()
  {
    return m_ID;
  }
  void addAttachment(const std::string& att)
  {
    m_Attachments.push_back(att);
  }
  void setLogbook(const std::string& logbook)
  {
    m_Logbook=logbook;
  }
  std::string getLogbook()
  {
    return m_Logbook;
  }
  void setSubdir(const std::string& subdir)
  {
    m_Subdir=subdir;
  }
  void setTextFile(const std::string& textfile);
  void addAttribute(const std::string& key, const std::string& value)
  {
    m_Attributes.emplace(key,value);
  }
  void setUserName(const std::string& user)
  {
    m_Uname=user;
  }
  void setPassword(const std::string& pass)
  {
    m_Upwd=do_crypt(pass);
  }
  void setQuoteInReply(const bool& quote)
  {
    m_quote_on_reply=quote;
  }
  void setSupressEmails(const bool& mail)
  {
    m_Suppress=mail;
  }
  void setEncoding(const int& encoding)
  {
    m_Encoding=encoding;
  }
  bool hasText()
  {
    return text_flag;
  }
  int SubmitElog();
  ~Elog()
  {
    for(std::size_t i = 0; i < buffer.size(); i++) free(buffer[i]);
  }
  void setText(const std::string& text)
  {
    m_Text=text;
    text_flag=true;
  }
  static int getMaxAttributes()
  {
    return m_maxNAttributes;
  }
  static int getMaxAttachments()
  {
    return m_maxAttachments;
  }
  std::string getText()
  {
    return m_Text;
  }
  std::map<std::string,std::string> getAttributes()
  {
    return m_Attributes;
  }
private:
  constexpr static const  int m_maxAttachments{50};
  constexpr static const int m_maxNAttributes{50};
  std::string retrieve_elog(std::map<std::string,std::string>& attrib,std::string& text);
  Type m_Type{New};
  Connector m_Connector;
  int m_ID{0};
  std::vector<std::string> m_Attachments;
  std::vector<int> att_size;
  std::vector<char*> buffer;
  std::string m_Logbook{""};
  std::string m_Subdir{""};
  std::string m_Textfile{""};
  bool text_flag{false};
  std::map<std::string,std::string> m_Attributes;
  std::string m_Uname{""};
  std::string m_Upwd{""};
  bool m_quote_on_reply{false};
  bool m_Suppress{false};
  std::string m_Text{""};
  int m_Encoding{0};
};

}
