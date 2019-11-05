#ifndef ElogServer_h
#define ElogServer_h
#include "Logbook.h"
#include <string>
#include <map>

namespace elogpp
{
class ElogServer
{
public :
    ElogServer();
    void SetHostname(std::string);
    std::string GetHostname();
    void SetPort(std::string);
    std::string GetPort();
    void SetSSL(bool);
    bool GetSSL();
    void SetName(std::string);
    std::string GetName();
    void SetDescription(std::string);
    std::string GetDescription();
    void Print();
    unsigned int GetServerNumber();
    void SetSubDir(std::string="");
    std::string GetSubDir();
    void AddLogbook(Logbook);
private:
    void SetServerNumber(const unsigned int&);
    std::string Hostname;
    std::string Port;
    std::string SubDir;
    bool SSL;
    std::string Name;
    std::string Description;
    inline static int ServerNumberTotal{0};
    unsigned int ServerNumber{0};
    std::map<std::string,Logbook>Logbooks;
};
}
#endif
