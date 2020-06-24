#include "ElogServer.hpp"
#include <string>
#include <iostream>
#include "Logbook.hpp"

namespace elogpp
{
  
void ElogServer::setHostname(const std::string& hostname)
{
  m_Hostname=hostname;
}

std::string ElogServer::getHostname()
{
  return m_Hostname;
}

void ElogServer::setPort(const std::string& port)
{
  m_Port=port;
}

std::string ElogServer::getPort()
{
  return m_Port;
}

void ElogServer::setSSL(const bool& ssl)
{
  m_SSL=ssl;
}

bool ElogServer::getSSL()
{
  return m_SSL;
}

void ElogServer::setName(const std::string& name)
{
  m_Name=name;
}

std::string ElogServer::getName()
{
  return m_Name;
}

void ElogServer::setDescription(const std::string& description)
{
  m_Description=description;
}

std::string ElogServer::getDescription()
{
  return m_Description;
}

void ElogServer::print()
{
    std::cout<<"Server "<<getServerNumber()<<" :\n"
             <<"\t-> Name : "<<getName()<<"\n"
             <<"\t-> Description : "<<getDescription()<<"\n"
             <<"\t-> Hostname : "<<getHostname()<<"\n"
             <<"\t-> Port : "<<getPort()<<"\n"
             <<"\t-> SSL : "<<getSSL()<<"\n"
             <<"\t-> Subdirectory : "<<getSubDir()<<"\n"
             <<"\t-> Logbook(s) : \n";
    for(std::map<std::string,Logbook>::iterator it=m_Logbooks.begin();it!=m_Logbooks.end();++it) 
    {
        it->second.print("\t\t");
    }
}

void ElogServer::setSubDir(const std::string& subdir)
{
  m_SubDir=subdir;  
}

std::string ElogServer::getSubDir()
{
  return m_SubDir;
}

void ElogServer::setServerNumber(const unsigned int& servernumber)
{
  m_ServerNumber=servernumber;   
}

unsigned int ElogServer::getServerNumber()
{
  return m_ServerNumber;
}

ElogServer::ElogServer()
{
  m_ServerNumberTotal++;  
  setServerNumber((unsigned int)(m_ServerNumberTotal));
}

void ElogServer::addLogbook(Logbook& logbook)
{
  m_Logbooks.insert(std::pair<std::string,Logbook>(logbook.getName(),std::move(logbook)));
}
}
