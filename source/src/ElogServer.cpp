#include "ElogServer.hpp"
#include <string>
#include <iostream>
#include "Logbook.hpp"

namespace elogpp
{
void ElogServer::SetHostname(std::string hostname)
{
     Hostname=hostname;
}
    
std::string ElogServer::GetHostname()
{
    return Hostname;
}
    
void ElogServer::SetPort(std::string port)
{
    Port=port;
}
    
std::string ElogServer::GetPort()
{
    return Port;
}
    
void ElogServer::SetSSL(bool ssl)
{
    SSL=ssl;
}
    
bool ElogServer::GetSSL()
{
    return SSL;
}

void ElogServer::SetName(std::string name)
{
    Name=name;
}
    
std::string ElogServer::GetName()
{
    return Name;
}
    
void ElogServer::SetDescription(std::string description)
{
    Description=description;
}
    
std::string ElogServer::GetDescription()
{
    return Description;
}

void ElogServer::Print()
{
    std::cout<<"Server "<<GetServerNumber()<<" :\n"
             <<"\t-> Name : "<<GetName()<<"\n"
             <<"\t-> Description : "<<GetDescription()<<"\n"
             <<"\t-> Hostname : "<<GetHostname()<<"\n"
             <<"\t-> Port : "<<GetPort()<<"\n"
             <<"\t-> SSL : "<<GetSSL()<<"\n"
             <<"\t-> Subdirectory : "<<GetSubDir()<<"\n"
             <<"\t-> Logbook(s) : \n";
    for(std::map<std::string,Logbook>::iterator it=Logbooks.begin();it!=Logbooks.end();++it) 
    {
        it->second.Print("\t\t");
    }
}

void ElogServer::SetSubDir(std::string subdir)
{
    SubDir=subdir;  
}

std::string ElogServer::GetSubDir()
{
    return SubDir;
}

void ElogServer::SetServerNumber(const unsigned int& servernumber)
{
    ServerNumber=servernumber;   
}

unsigned int ElogServer::GetServerNumber()
{
    return ServerNumber;
}

ElogServer::ElogServer()
{
  ServerNumberTotal++;  
  SetServerNumber((unsigned int)(ServerNumberTotal));
}

void ElogServer::AddLogbook(Logbook logbook)
{
   Logbooks.insert(std::pair<std::string,Logbook>(logbook.GetName(),std::move(logbook)));
}
}
