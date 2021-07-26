#pragma once

#include <string>

namespace elogpp
{

class Logbook
{
public:
  void setName(const std::string&);
  std::string getName();
  void setDescription(const std::string&);
  std::string getDescription();
  void print(const std::string& shift="");
private:
  std::string m_Description;
  std::string m_Name;
};

}
