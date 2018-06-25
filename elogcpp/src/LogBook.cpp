#include "Logbook.h"
#include <iostream>
#include <string>

namespace elogpp
{
void Logbook::SetName(std::string name)
{
    Name=name;   
}
    
std::string Logbook::GetName()
{
    return Name;
}
    
void Logbook::SetDescription(std::string description)
{
    Description=description;
}

std::string Logbook::GetDescription()
{
    return Description;
}

void Logbook::Print(std::string t)
{
    std::cout<<t<<"Logbook : \n"
             <<t<<"\t* Name : "<<GetName()<<"\n"
             <<t<<"\t* Description : "<<GetDescription()<<"\n";
}
}
