#include "Manager.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Entry.hpp"

namespace cxx::elog
{

Entry Manager::createEntry()
{
  Entry entry;
  return std::move(entry);
}

void Manager::setVerbosity(const bool verbosity)
{
  m_Verbose=verbosity;
}

bool Manager::isVerbose()
{
  return m_Verbose;
}

}
