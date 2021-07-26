#include "ElogConfig.hpp"
#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include <dotenv.h>

namespace elogpp
{

std::string ElogConfig::NormalizePath(const std::string& path)
{
  std::size_t found = path.find_last_of("/\\");
  if(found==std::string::npos) return path;
  else if(found==path.size()-1) return path.substr(0,found);
  else return path;
}

ElogUser ElogConfig::getUser(const std::string& user)
{
  if(m_hasBeenLoaded==false) openFile();
  if(hasUser(user)) return m_Users[user];
  else throw;
}

ElogServer ElogConfig::getServer(const std::string& server)
{
  if(m_hasBeenLoaded==false) openFile();
  if(hasServer(server)) return m_Servers[server];
  else throw;
}

void ElogConfig::setPath(const std::string& path)
{
  m_hasBeenLoaded=false;
  m_Path=path;
}

void ElogConfig::setConfigFile(const std::string& file)
{
  m_hasBeenLoaded=false;
  m_ConfigFile=file;
}

std::string ElogConfig::getPath()
{
  return m_Path;
}

std::string ElogConfig::getConfigFile()
{
  return m_ConfigFile;
}

bool ElogConfig::hasUser(const std::string& user)
{
  if(m_hasBeenLoaded==false) openFile();
  if(m_Users.find(user)!=m_Users.end()) return true;
  else return false;
}

bool ElogConfig::hasServer(const std::string& server)
{
  if(m_hasBeenLoaded==false) openFile();
  if(m_Servers.find(server)!=m_Servers.end()) return true;
  else return false;
}

void ElogConfig::openFile()
{
  dotenv::init();
  Json::CharReaderBuilder builder;
  Json::Value obj;   // will contain the root value after parsing.
  if(m_Path.empty()) m_Path = NormalizePath(dotenv::getenv("ELOG_CONFIG_PATH", "."));
  if(m_ConfigFile.empty()) m_ConfigFile = dotenv::getenv("ELOG_CONFIG_FILE", "ElogConfig.json");
  std::string errs;
  std::ifstream ConfFile((m_Path+"/"+m_ConfigFile).c_str(),std::ifstream::binary);
  std::cout<<m_Path+"/"+m_ConfigFile<<std::endl;
  bool ok = Json::parseFromStream(builder,ConfFile,&obj,&errs);
  if( !ok )
  {
    std::cout  << errs << "\n";
  }
  extractElogServersInfos(obj);
  extractElogUsersInfos(obj);
  m_hasBeenLoaded = true;
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
  if(m_hasBeenLoaded==false) openFile();
  if(server!="")
  {
    if(m_Servers.find(server)!=m_Servers.end()) m_Servers[server].print();
    else std::cout<<"Server with name "<<server<<" unknown ! Please check your configuration file ! \n";
  }
  else for(std::map<std::string,ElogServer>::iterator it=m_Servers.begin();it!=m_Servers.end();++it) it->second.print();
}

void ElogConfig::printUser(const std::string& user)
{
  if(m_hasBeenLoaded==false) openFile();
  if(user!="")
  {
    if(m_Users.find(user)!=m_Users.end()) m_Users[user].print();
    else std::cout<<"Server with name "<<user<<" unknown ! Please check your configuration file ! \n";
  }
  else for(std::map<std::string,ElogUser>::iterator it=m_Users.begin();it!=m_Users.end();++it) it->second.print();
}

}
