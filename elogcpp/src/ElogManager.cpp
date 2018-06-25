#include "ElogManager.h"
#include <map>
#include <string>
#include <memory>
#include <iostream>
#include "ElogServer.h"
#include "ElogUser.h"
#include "ElogEntry.h"

namespace elogpp
{
ElogEntry ElogManager::CreateEntry()
{
    ElogEntry entry;
    return std::move(entry);
}

/*void ElogManager::SetDefaultUser(std::string user)
{
    if(Users.find(user)!=Users.end()) DefaultUser=user;
    else std::cout<<"User "<<user<<" unknow ! Please check your configuration file !\n";
}*/


void ElogManager::PrintServer(std::string server)
{
    conf.PrintServer(server);
}

void ElogManager::PrintUser(std::string user)
{
    conf.PrintUser();
}

void ElogManager::SetVerbosity(bool verbosity)
{
    Verbose=verbosity;
}

bool ElogManager::IsVerbose()
{
    return Verbose;
}

void ElogManager::Print()
{
    PrintServer();
    PrintUser();
}

}
