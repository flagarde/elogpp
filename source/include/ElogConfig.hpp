#pragma once
#include "ElogServer.hpp"
#include "ElogUser.hpp"
#include "json/json.h"
#include <map>
#include <string>

namespace elogpp
{
  
class ElogConfig
{
public:
    ElogConfig();
    void printServer(const std::string& server="");
    void printUser(const std::string& user="");
    bool hasUser(const std::string&);
    bool hasServer(const std::string&);
    ElogUser getUser(const std::string&);
    ElogServer getServer(const std::string&);
private :
    std::map<std::string,ElogServer> m_Servers;
    std::map<std::string,ElogUser> m_Users;
    std::string getEnvVar(const std::string &);
    Json::Value openJSONFile(const std::string&);
    void extractElogServersInfos(const Json::Value& root);
    void extractElogUsersInfos(const Json::Value& root);
};

}
