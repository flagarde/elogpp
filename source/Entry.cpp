#include "Entry.hpp"
#include <iostream>
#include <vector>

namespace cxx::elog
{

std::string Entry::supressSpaces(const std::string& part)
{
  std::string ret=part;
  if(ret==""|| ret==" "||ret==" \n"||ret=="\n ") return "";
  ret=std::string(ret.rbegin(),ret.rend());
  while(!isgraph(ret[0]))
  {
    if(ret.size()==1) return "";
    else ret=ret.erase(0,1);
  }
  ret=std::string(ret.rbegin(),ret.rend());
  while(!isgraph(ret[0]))
  {
    ret=ret.erase(0,1);
  }
  return std::move(ret);
}

Entry& Entry::receiveEntry(const int id)
{
  m_Elog.setType(Download);
  m_Elog.setID(id);
  m_Elog.SubmitElog();
  return *this;
}

std::string Entry::getLastID()
{
  m_Elog.setType(Download);
  m_Elog.setID(-1);
  m_Elog.SubmitElog();
  return std::to_string(m_Elog.getID());
}

Entry& Entry::receiveEntry(const std::string& id)
{
  if(id=="last")
  {
    return receiveEntry(std::stoi(getLastID()));
  }
  return receiveEntry(std::stoi(id));
}

Entry & Entry::edit(const int id)
{
  m_Elog.setType(Edit);
  m_Elog.setID(id);
  return *this;
}

Entry & Entry::edit(const std::string& id)
{
  return edit(std::stoi(id));
}

Entry & Entry::replyTo(const int id,const std::string& option)
{
  m_Elog.setID(id);
  m_Elog.setType(Reply);
  return *this;
}

Entry & Entry::replyTo(const std::string& id,const std::string& option)
{
  if(id=="last")
  {
    return replyTo(std::stoi(getLastID()),option);
  }
  return replyTo(std::stoi(id),option);
}

Entry& Entry::user(const User& user)
{
  m_Elog.setUserName(user.getName());
  m_Elog.setPassword(user.getPassword());
  return *this;
}

bool Entry::send(const std::string& option)
{
  return m_Elog.SubmitElog();
}

Entry& Entry::to(const Server& server,const std::string& logbook)
{
  m_Elog.setHostname(server.getHostname());
  m_Elog.setLogbook(logbook);
  m_Elog.setPort(server.getPort());
  m_Elog.setSSL(server.getSSL());
  m_Elog.setSubdir(server.getSubDir());
  m_Elog.setCACertificateFile(server.getCACertificateFile());
  m_Elog.setCACertificatePath(server.getCACertificatePath());
  return *this;
}

void Entry::setAttribute(const std::string& attribute,const std::string& value)
{
  m_Elog.addAttribute(attribute,value);
}

std::string Entry::getAttribute(const std::string& attribute)
{
  return m_Elog.getAttributes()[attribute];
}

bool Entry::isAttribute(const std::string& attribute)
{
  if(m_Elog.getAttributes().find(attribute)!=m_Elog.getAttributes().end()) return true;
  else return false;
}
/*
bool Entry::IsAttributeEmpty(const std::string& attribute)
{
    return message.IsAttributeEmpty(attribute);
}*/

void Entry::addAttachment(const std::string& attachment)
{
  m_Elog.addAttachment(attachment);
}

void Entry::setMessage(const std::string& message)
{
  m_Elog.setText(message);
}

void Entry::addFileMessage(const std::string& filemessage)
{
  m_Elog.setTextFile(filemessage);
}

void Entry::print()
{
  std::map<std::string,std::string> attributes=m_Elog.getAttributes();
  for(std::map<std::string,std::string>::iterator it=attributes.begin();it!=attributes.end();++it)
  {
    std::cout<<it->first<<":"<<it->second<<std::endl;
  }
  std::cout<<"========================================"<<std::endl;
  std::cout<<m_Elog.getText()<<std::endl;
}
/*
bool Entry::hasAttachment()
{
  return message.HasAttachment();
}

bool Entry::HasAttributes()
{
    return message.HasAttributes();
}

bool Entry::HasFileMessage()
{
    return message.HasFileMessage();
}


bool Entry::HasTextMessage()
{
    return message.HasTextMessage();
}*/

std::vector<std::string> Entry::getAttachments()
{
  return m_Elog.getAttachments();
}

std::map<std::string, std::string> Entry::getAttributes()
{
  return m_Elog.getAttributes();
}
/*
std::string Entry::GetFileMessage()
{
  return message.GetFileMessage();
}
*/
std::string Entry::getText()
{
  return m_Elog.getText();
}

}
