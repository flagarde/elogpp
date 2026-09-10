#pragma once
#include <string>
#include <string_view>

namespace cxx::elog
{

class Logbook
{
public:
  Logbook()=default;
  Logbook(const std::string_view name, const std::string_view description) : m_Name(name),m_Description(description) {}
  void setName(const std::string_view name) { m_Name = name; }
  std::string getName() const noexcept { return m_Name; }
  void setDescription(const std::string_view description ) { m_Description = description; }
  std::string getDescription() const noexcept { return m_Description; }
private:
  std::string m_Description;
  std::string m_Name;
};

}
