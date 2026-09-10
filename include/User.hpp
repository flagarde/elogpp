#pragma once
#include <string>
#include <string_view>

namespace cxx::elog
{

class User
{
public:
  User() noexcept =default;
  User(const std::string_view name,const std::string_view password) : m_Name(name),m_Password(password) {}
  void setName(const std::string_view name) { m_Name=name; }
  std::string getName() const noexcept { return m_Name; }
  void setPassword(const std::string_view password) { m_Password=password; }
  std::string getPassword() const noexcept { return m_Password; }
private:
  std::string m_Name;
  std::string m_Password;
};

}
