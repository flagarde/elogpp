#pragma once

#include <string>
#include <map>

#include "Logbook.hpp"

namespace elogpp
{

class ElogServer
{
public :
    ElogServer();
    void setHostname(const std::string&);
    std::string getHostname();
    void setPort(const std::string&);
    std::string getPort();
    void setSSL(const bool&);
    bool getSSL();
    void setName(const std::string&);
    std::string getName();
    void setDescription(const std::string&);
    std::string getDescription();
    void print();
    unsigned int getServerNumber();
    void setSubDir(const std::string& c="");
    std::string getSubDir();
    void addLogbook(Logbook&);
private:
    void setServerNumber(const unsigned int&);
    std::string m_Hostname;
    std::string m_Port;
    std::string m_SubDir;
    bool m_SSL{false};
    std::string m_Name;
    std::string m_Description;
    static int m_ServerNumberTotal;
    unsigned int m_ServerNumber{0};
    std::map<std::string,Logbook> m_Logbooks;
};

}
