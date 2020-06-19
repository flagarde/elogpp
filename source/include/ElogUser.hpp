#ifndef ElogUser_h
#define ElogUser_h
#include <string>

namespace elogpp
{
class ElogUser
{
public:
    void SetName(std::string);
    std::string GetName();
    void SetPassword(std::string);
    std::string GetPassword();
    void Print();
private:
    std::string Name{""};
    std::string Password{""};
};
}
#endif
