#include "ElogUser.h"
#include <iostream>
#include <string>

namespace elogpp
{
void ElogUser::SetName(std::string name)
{
    Name=name;   
}

std::string ElogUser::GetName()
{
    return Name;
}

void ElogUser::SetPassword(std::string password)
{
    Password=password;
}

std::string ElogUser::GetPassword()
{
    return Password;
}

void ElogUser::Print()
{
    std::cout<<"User :\n"
             <<"\tName: "<<GetName()<<"\n"
             <<"\tPassword: "<<GetPassword()<<"\n";
}
}
