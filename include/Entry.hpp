#pragma once
#include "Elog.hpp"
#include "User.hpp"
#include "Server.hpp"

enum class Type{Reply,Download,Option};

namespace cxx::elog
{

class Entry
{
public:
  Entry() noexcept = default;
  ~Entry() = default;
  Entry(const Entry&) = delete;
  Entry& operator=(const Entry&) = delete;
  Entry(Entry&& other) noexcept = default;
  Entry& operator=(Entry&& other) noexcept = default;
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
  Entry& user(const User& user);
  Entry& replyTo(const std::string&,const std::string& pa="");
  Entry& replyTo(const int,const std::string& pa="");
  Entry& edit(const int);
  Entry& edit(const std::string&);
  Entry& to(const Server& server,const std::string& logbook);
  Entry& receiveEntry(const std::string&);
  Entry& receiveEntry(const int);
  std::string getLastID();
private :
  std::string supressSpaces(const std::string&);
  Elog m_Elog;
};

}
