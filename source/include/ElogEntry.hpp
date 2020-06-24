#pragma once
#include "ElogConfig.hpp"
#include "Elog.hpp"

enum class Type{Reply,Download,Option}; 

namespace elogpp
{
  
class ElogEntry
{
public:
  void setAttribute(const std::string&,const std::string&);
  std::string getAttribute(const std::string&);
  bool isAttribute(const std::string&);
 // bool isAttributeEmpty(const std::string&);
  void addAttachment(const std::string&);
  void setMessage(const std::string&);
  void addFileMessage(const std::string&);
  void print();
 /* bool hasAttachment();
  bool hasAttributes();
  bool hasFileMessage();
  bool hasTextMessage();*/
  std::vector<std::string> getAttachments();
  std::map<std::string,std::string> getAttributes();
 /* std::string getFileMessage();*/
  std::string getText();
  bool send(const std::string& param="");
  ElogEntry& user(const std::string&);
  ElogEntry& replyTo(const std::string&,const std::string& pa="");
  ElogEntry& replyTo(const int&,const std::string& pa="");
  ElogEntry& edit(const int&);
  ElogEntry& edit(const std::string&);
  ElogEntry& to(const std::string&,const std::string&);
  ElogEntry& receiveEntry(const std::string&);
  ElogEntry& receiveEntry(const int&);
private :
  std::string getLastID();
  std::string supressSpaces(const std::string&);
  ElogConfig m_Conf;
  Elog m_Elog;
};

}
