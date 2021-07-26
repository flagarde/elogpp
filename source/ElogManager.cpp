#include "ElogManager.hpp"
#include "ElogServer.hpp"
#include "ElogUser.hpp"
#include "ElogEntry.hpp"

#include <string>

namespace elogpp
{

ElogEntry ElogManager::createEntry()
{
  ElogEntry entry;
  return std::move(entry);
}

void ElogManager::setPath(const std::string& path)
{
  m_Conf.setPath(path);
}

void ElogManager::setConfigFile(const std::string& file)
{
  m_Conf.setConfigFile(file);
}

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
