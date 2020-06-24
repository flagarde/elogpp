#include "ElogEntry.hpp"
#include <iostream>
#include <vector>

namespace elogpp
{

std::string ElogEntry::supressSpaces(const std::string& part)
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

ElogEntry& ElogEntry::receiveEntry(const int& id)
{
  m_Elog.setType(Download);
  m_Elog.setID(id);
  m_Elog.SubmitElog();
  return *this;
}

std::string ElogEntry::getLastID()
{
  m_Elog.setType(Download);
  m_Elog.setID(-1);
  m_Elog.SubmitElog();
  return std::to_string(m_Elog.getID());
}

ElogEntry& ElogEntry::receiveEntry(const std::string& id)
{
  if(id=="last")
  {
    return receiveEntry(std::stoi(getLastID()));
  }
  return receiveEntry(std::stoi(id));
}
    
ElogEntry & ElogEntry::edit(const int& id)
{
  m_Elog.setType(Edit);
  m_Elog.setID(id);
  return *this;
}

ElogEntry & ElogEntry::edit(const std::string& id)
{
  return edit(std::stoi(id));
}

ElogEntry & ElogEntry::replyTo(const int& id,const std::string& option)
{
  m_Elog.setID(id);
  m_Elog.setType(Reply);
  return *this;
}

ElogEntry & ElogEntry::replyTo(const std::string& id,const std::string& option)
{
  if(id=="last")
  {
    return replyTo(std::stoi(getLastID()),option);
  }
  return replyTo(std::stoi(id),option);
}

ElogEntry& ElogEntry::user(const std::string& user)
{
  if(m_Conf.hasUser(user))
  {
    m_Elog.setUserName(m_Conf.getUser(user).getName());
    m_Elog.setPassword(m_Conf.getUser(user).getPassword());
  }
  else std::cout<<"User "<<user<<" unknown ! Please check your configuration files !\n";
  return *this;
}

bool ElogEntry::send(const std::string& option)
{
  return m_Elog.SubmitElog();
}

ElogEntry& ElogEntry::to(const std::string& server,const std::string& logbook)
{
  if(m_Conf.hasServer(server))
  {
    m_Elog.setHostname(m_Conf.getServer(server).getHostname());
    m_Elog.setLogbook(logbook);
    m_Elog.setPort(std::stoi(m_Conf.getServer(server).getPort()));
    m_Elog.setSSL(m_Conf.getServer(server).getSSL());
    m_Elog.setSubdir(m_Conf.getServer(server).getSubDir());
  }
  else
  {
    std::cout<<"Server "<<server<<" unknown ! Please check your configuration files !\n";
    std::exit(2);
  }
  return *this;
}

void ElogEntry::setAttribute(const std::string& attribute,const std::string& value)
{
  m_Elog.addAttribute(attribute,value);
}

std::string ElogEntry::getAttribute(const std::string& attribute)
{
  return m_Elog.getAttributes()[attribute];
}

bool ElogEntry::isAttribute(const std::string& attribute) 
{
  if(m_Elog.getAttributes().find(attribute)!=m_Elog.getAttributes().end()) return true;
  else return false;
}
/*
bool ElogEntry::IsAttributeEmpty(const std::string& attribute)
{
    return message.IsAttributeEmpty(attribute);
}*/

void ElogEntry::addAttachment(const std::string& attachment)
{
  m_Elog.addAttachment(attachment);
}

void ElogEntry::setMessage(const std::string& message)
{
  m_Elog.setText(message);
}

void ElogEntry::addFileMessage(const std::string& filemessage)
{
  m_Elog.setTextFile(filemessage);
}

void ElogEntry::print()
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
bool ElogEntry::hasAttachment()
{
  return message.HasAttachment();
}

bool ElogEntry::HasAttributes()
{
    return message.HasAttributes();
}

bool ElogEntry::HasFileMessage()
{
    return message.HasFileMessage();
}


bool ElogEntry::HasTextMessage()
{
    return message.HasTextMessage();
}*/

std::vector<std::string> ElogEntry::getAttachments()
{
  return m_Elog.getAttachments();
}

std::map<std::string, std::string> ElogEntry::getAttributes()
{
  return m_Elog.getAttributes();
}
/*
std::string ElogEntry::GetFileMessage()
{
  return message.GetFileMessage();
}
*/
std::string ElogEntry::getText()
{
  return m_Elog.getText();
}

}
