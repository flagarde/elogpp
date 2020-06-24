#include "ElogUser.hpp"
#include <iostream>
#include <string>

namespace elogpp
{
void ElogUser::setName(const std::string& name)
{
  m_Name=name;   
}

std::string ElogUser::getName()
{
  return m_Name;
}

void ElogUser::setPassword(const std::string& password)
{
  m_Password=password;
}

std::string ElogUser::getPassword()
{
  return m_Password;
}

void ElogUser::print()
{
  std::cout<<"User :\n"<<"\tName: "<<getName()<<"\n"<<"\tPassword: "<<getPassword()<<"\n";
}

}
