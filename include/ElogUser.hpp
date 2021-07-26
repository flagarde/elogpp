#pragma once

#include <string>

namespace elogpp
{

class ElogUser
{
public:
    void setName(const std::string&);
    std::string getName();
    void setPassword(const std::string&);
    std::string getPassword();
    void print();
private:
    std::string m_Name;
    std::string m_Password;
};

}
