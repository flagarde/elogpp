#pragma once
#include <string>

namespace elogpp
{
class Logbook
{
public:
    void SetName(std::string);
    std::string GetName();
    void SetDescription(std::string);
    std::string GetDescription();
    void Print(std::string="");
private:
    std::string Description{""};
    std::string Name{""};
};
}
