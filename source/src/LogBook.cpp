#include "Logbook.hpp"
#include <iostream>
#include <string>

namespace elogpp
{
  
void Logbook::setName(const std::string& name)
{
  m_Name=name;   
}

std::string Logbook::getName()
{
  return m_Name;
}

void Logbook::setDescription(const std::string& description)
{
  m_Description=description;
}

std::string Logbook::getDescription()
{
    return m_Description;
}

void Logbook::print(const std::string& t)
{
    std::cout<<t<<"Logbook : \n"
             <<t<<"\t* Name : "<<getName()<<"\n"
             <<t<<"\t* Description : "<<getDescription()<<"\n";
}

}
