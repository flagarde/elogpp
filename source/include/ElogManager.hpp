#pragma once
#include <string>
#include <map>
#include "ElogCommand.hpp"
#include "ElogConfig.hpp"
#include "ElogEntry.hpp"

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
