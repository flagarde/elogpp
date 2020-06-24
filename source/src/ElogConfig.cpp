#include "ElogConfig.hpp"
#include <string>
#include <cstdlib>
#include "json/json.h"
#include <fstream>
#include <iostream>

namespace elogpp
{
  
ElogUser ElogConfig::getUser(const std::string& user)
{
  if(hasUser(user)) return m_Users[user];
  else throw;
}

ElogServer ElogConfig::getServer(const std::string& server)
{
  if(hasServer(server)) return m_Servers[server];
  else throw;
}

bool ElogConfig::hasUser(const std::string& user)
{
  if(m_Users.find(user)!=m_Users.end()) return true;
  else return false;
}

bool ElogConfig::hasServer(const std::string& server)
{
  if(m_Servers.find(server)!=m_Servers.end()) return true;
  else return false;
}

ElogConfig::ElogConfig()
{
  extractElogServersInfos(openJSONFile("ElogServerConfFile"));
  extractElogUsersInfos(openJSONFile("ElogServerConfFile"));
}

std::string ElogConfig::getEnvVar(const std::string& key )
{
  if(std::getenv( key.c_str() )==nullptr) return "";
  else return std::string(std::getenv( key.c_str() ));
}

Json::Value ElogConfig::openJSONFile(const std::string& envVar)
{
  Json::CharReaderBuilder builder;
  Json::Value obj;   // will contain the root value after parsing.
  std::string errs;
  std::string FileName=getEnvVar(envVar.c_str());
  if(FileName=="")
  {
    std::cout<<"Please add "<<envVar<<" as variable environment ! \n";
    std::exit(2);
  }
  std::ifstream ConfFile(FileName.c_str(),std::ifstream::binary);
  bool ok = Json::parseFromStream(builder,ConfFile,&obj,&errs);
  if ( !ok )
  {
    std::cout  << errs << "\n";
  }
  return obj;
}

void ElogConfig::extractElogUsersInfos(const Json::Value& root)
{
  const Json::Value& elogusers = root["ElogUsers"];
  for(unsigned int i=0; i<elogusers.size();++i)
  {
    ElogUser user;
    user.setName(elogusers[i]["Name"].asString());
    user.setPassword(elogusers[i]["Password"].asString());
    m_Users.insert(std::pair<std::string,ElogUser>(user.getName(),std::move(user)));
  }
}

void ElogConfig::extractElogServersInfos(const Json::Value& root)
{
  const Json::Value& elogservers = root["ElogServers"];
  for(unsigned int i=0; i<elogservers.size();++i)
  {
    ElogServer server;
    server.setName(elogservers[i]["Name"].asString());
    server.setDescription(elogservers[i]["Description"].asString());
    server.setHostname(elogservers[i]["Hostname"].asString());
    server.setPort(elogservers[i]["Port"].asString());
    server.setSSL(elogservers[i]["SSL"].asBool());
    server.setSubDir(elogservers[i]["SubDir"].asString());
    const Json::Value& logbooks = elogservers[i]["Logbooks"]; 
    for (unsigned int j = 0; j < logbooks.size(); ++j)
    {
      Logbook logbook;
      logbook.setName(logbooks[j]["Name"].asString());
      logbook.setDescription(logbooks[j]["Description"].asString());
      server.addLogbook(logbook);
    }
    m_Servers.insert(std::pair<std::string,ElogServer>(server.getName(),std::move(server)));
  }
}

void ElogConfig::printServer(const std::string& server)
{
  if(server!="")
  {
    if(m_Servers.find(server)!=m_Servers.end()) m_Servers[server].print();
    else std::cout<<"Server with name "<<server<<" unknown ! Please check your configuration file ! \n";
  }
  else for(std::map<std::string,ElogServer>::iterator it=m_Servers.begin();it!=m_Servers.end();++it) it->second.print();
}

void ElogConfig::printUser(const std::string& user)
{
  if(user!="")
  {
    if(m_Users.find(user)!=m_Users.end()) m_Users[user].print();
    else std::cout<<"Server with name "<<user<<" unknown ! Please check your configuration file ! \n";
  }
  else for(std::map<std::string,ElogUser>::iterator it=m_Users.begin();it!=m_Users.end();++it) it->second.print();
}

}
