#include "Server.hpp"
#include <iostream>

namespace cxx::elog
{

void Server::print()
{
    std::cout<<"Server:\n"
             <<"\t-> Name : "<<getName()<<"\n"
             <<"\t-> Description : "<<getDescription()<<"\n"
             <<"\t-> Hostname : "<<getHostname()<<"\n"
             <<"\t-> Port : "<<getPort()<<"\n"
             <<"\t-> SSL : "<<getSSL()<<"\n"
             <<"\t-> Subdirectory : "<<getSubDir()<<"\n";
}

}
