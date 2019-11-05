#ifndef ElogManager_h
#define ElogManager_h
#include <string>
#include <map>
#include "ElogCommand.h"
#include "ElogConfig.h"
#include "ElogEntry.h"

namespace elogpp
{
class ElogManager
{
public:
    void PrintServer(std::string="");
    void PrintUser(std::string="");
    void Print();
    void SetVerbosity(bool);
    bool IsVerbose();
    //void SetDefaultUser(std::string);
    ElogEntry CreateEntry();
private:
    ElogConfig conf;
    bool Verbose{false};
    std::string DefaultUser{""};
    ElogCommand Command;
};
}
#endif
