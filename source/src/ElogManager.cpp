#include "ElogManager.hpp"
#include <map>
#include <string>
#include <memory>
#include <iostream>
#include "ElogServer.hpp"
#include "ElogUser.hpp"
#include "ElogEntry.hpp"

namespace elogpp
{
  
ElogEntry ElogManager::createEntry()
{
  ElogEntry entry;
  return std::move(entry);
}

/*void ElogManager::SetDefaultUser(std::string user)
{
    if(Users.find(user)!=Users.end()) DefaultUser=user;
    else std::cout<<"User "<<user<<" unknow ! Please check your configuration file !\n";
}*/


void ElogManager::printServer(const std::string& server)
{
  m_Conf.printServer(server);
}

void ElogManager::printUser(const std::string& user)
{
  m_Conf.printUser();
}

void ElogManager::setVerbosity(const bool& verbosity)
{
  m_Verbose=verbosity;
}

bool ElogManager::isVerbose()
{
  return m_Verbose;
}

void ElogManager::print()
{
  printServer();
  printUser();
}

}
