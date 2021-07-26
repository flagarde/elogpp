#pragma once

#include <string>

#include "ElogConfig.hpp"
#include "ElogEntry.hpp"

namespace elogpp
{

class ElogManager
{
public:
  void printServer(const std::string& shift="");
  void printUser(const std::string& shift="");
  void print();
  void setPath(const std::string&);
  void setConfigFile(const std::string&);
  std::string getPath();
  std::string getConfigFile();
  void setVerbosity(const bool&);
  bool isVerbose();
  ElogEntry createEntry();

private:
  ElogConfig m_Conf;
  bool m_Verbose{false};
  std::string m_DefaultUser;
};

}
