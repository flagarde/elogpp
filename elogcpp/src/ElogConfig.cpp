#include "ElogConfig.h"
#include <string>
#include <cstdlib>
#include "json/json.h"
#include <fstream>
#include <iostream>

namespace elogpp
{
ElogUser& ElogConfig::GetUser(std::string user)
{
    if(HasUser(user)) return Users[user];
}

ElogServer& ElogConfig::GetServer(std::string server)
{
    if(HasServer(server)) return Servers[server];
}


bool ElogConfig::HasUser(std::string user)
{
    if(Users.find(user)!=Users.end()) return true;
    else return false;
}

bool ElogConfig::HasServer(std::string server)
{
    if(Servers.find(server)!=Servers.end()) return true;
    else return false;
}

ElogConfig::ElogConfig()
{
   ExtractElogServersInfos(OpenJSONFile("ElogServerConfFile"));
   ExtractElogUsersInfos(OpenJSONFile("ElogServerConfFile"));
}

std::string ElogConfig::getEnvVar( std::string const & key )
{
    if(std::getenv( key.c_str() )==nullptr) return "";
    else return std::string(std::getenv( key.c_str() ));
}

Json::Value ElogConfig::OpenJSONFile(std::string envVar)
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

void ElogConfig::ExtractElogUsersInfos(Json::Value root)
{
    const Json::Value& elogusers = root["ElogUsers"];
    for(unsigned int i=0; i<elogusers.size();++i)
    {
        ElogUser user;
        user.SetName(elogusers[i]["Name"].asString());
        user.SetPassword(elogusers[i]["Password"].asString());
        Users.insert(std::pair<std::string,ElogUser>(user.GetName(),std::move(user)));
    }
}

void ElogConfig::ExtractElogServersInfos(Json::Value root)
{
    const Json::Value& elogservers = root["ElogServers"];
    for(unsigned int i=0; i<elogservers.size();++i)
    {
        ElogServer server;
        server.SetName(elogservers[i]["Name"].asString());
        server.SetDescription(elogservers[i]["Description"].asString());
        server.SetHostname(elogservers[i]["Hostname"].asString());
        server.SetPort(elogservers[i]["Port"].asString());
        server.SetSSL(elogservers[i]["SSL"].asBool());
        server.SetSubDir(elogservers[i]["SubDir"].asString());
        const Json::Value& logbooks = elogservers[i]["Logbooks"]; 
        for (unsigned int j = 0; j < logbooks.size(); ++j)
        {
            Logbook logbook;
            logbook.SetName(logbooks[j]["Name"].asString());
            logbook.SetDescription(logbooks[j]["Description"].asString());
            server.AddLogbook(logbook);
        }
        Servers.insert(std::pair<std::string,ElogServer>(server.GetName(),std::move(server)));
    }
}

void ElogConfig::PrintServer(std::string server)
{
    if(server!="")
    {
        if(Servers.find(server)!=Servers.end()) Servers[server].Print();
        else std::cout<<"Server with name "<<server<<" unknown ! Please check your configuration file ! \n";
    }
    else for(std::map<std::string,ElogServer>::iterator it=Servers.begin();it!=Servers.end();++it) it->second.Print();
}

void ElogConfig::PrintUser(std::string user)
{
    if(user!="")
    {
        if(Users.find(user)!=Users.end()) Users[user].Print();
        else std::cout<<"Server with name "<<user<<" unknown ! Please check your configuration file ! \n";
    }
    else for(std::map<std::string,ElogUser>::iterator it=Users.begin();it!=Users.end();++it) it->second.Print();
}
}
