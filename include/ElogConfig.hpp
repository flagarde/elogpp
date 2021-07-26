#pragma once

#include <map>
#include <string>

#include "ElogServer.hpp"
#include "ElogUser.hpp"

#include <json/json.h>

namespace elogpp
{

class ElogConfig
{
public:
  void printServer(const std::string& server="");
  void printUser(const std::string& user="");
  bool hasUser(const std::string&);
  bool hasServer(const std::string&);
  ElogUser getUser(const std::string&);
  ElogServer getServer(const std::string&);
  void setPath(const std::string&);
  void setConfigFile(const std::string&);
  std::string getPath();
  std::string getConfigFile();

private :
  std::string NormalizePath(const std::string& path);
  std::map<std::string,ElogServer> m_Servers;
  std::map<std::string,ElogUser> m_Users;
  void openFile();
  void extractElogServersInfos(const Json::Value& root);
  void extractElogUsersInfos(const Json::Value& root);
  std::string m_Path;
  std::string m_ConfigFile;
  bool m_hasBeenLoaded{false};
};

}
