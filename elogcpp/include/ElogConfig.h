#ifndef ElogConfig_h
#define ElogConfig_h
#include "ElogServer.h"
#include "ElogUser.h"
#include "json/json.h"
#include <map>
#include <string>

namespace elogpp
{
class ElogConfig
{
public:
    ElogConfig();
    void PrintServer(std::string="");
    void PrintUser(std::string="");
    bool HasUser(std::string);
    bool HasServer(std::string);
    ElogUser& GetUser(std::string);
    ElogServer& GetServer(std::string);
private :
    std::map<std::string,ElogServer> Servers;
    std::map<std::string,ElogUser>Users;
    std::string getEnvVar(std::string const &);
    Json::Value OpenJSONFile(std::string);
    void ExtractElogServersInfos(Json::Value root);
    void ExtractElogUsersInfos(Json::Value root);
};
}
#endif
