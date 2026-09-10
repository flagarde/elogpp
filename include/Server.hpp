#pragma once
#include "Logbook.hpp"
#include <string>
#include <string_view>
#include <map>
#include <cstdint>
#include <filesystem>

namespace cxx::elog
{

class Server
{
public :
  Server() = default;
  Server& setHostname(const std::string_view hostname)
  {
    m_Hostname=hostname;
    return *this;
  }
  std::string_view getHostname() const noexcept { return m_Hostname; }
  Server& setPort(const std::string_view port)
  {
    m_Port=std::stoi(std::string(port));
    return *this;
  }
  std::uint16_t getPort() const noexcept { return m_Port; }
  Server& setSSL(const bool& ssl)
  {
    m_SSL=ssl;
    return *this;
  }
  bool getSSL() const noexcept { return m_SSL; }
  Server& setName(const std::string_view name)
  {
    m_Name=name;
    return *this;
  }
  std::string_view getName() const noexcept { return m_Name; }
  Server& setDescription(const std::string_view description)
  {
    m_Description=description;
    return *this;
  }
  std::string_view getDescription() const noexcept { return m_Description; }
  void print();

  Server& setSubDir(const std::string_view& c="")
  {
    m_SubDir=c;
    return *this;
  }
  std::string_view getSubDir() const noexcept { return m_SubDir; }
  void addLogbook(const std::string_view name, const std::string_view description)
  {
    m_Logbooks.insert(std::pair<std::string,Logbook>(std::string(name),Logbook(name, description)));
  }

  Server& setCACertificateFile(const std::filesystem::path& path)
  {
    s_CACertFilePath = path;
    return *this;
  }

  std::filesystem::path getCACertificateFile() const noexcept { return s_CACertFilePath; }

  Server& setCACertificatePath(const std::filesystem::path& path)
  {
    s_CACertDirPath = path;
    return *this;
  }

  std::filesystem::path getCACertificatePath() const noexcept { return s_CACertDirPath; }

private:
  std::string m_Hostname;
  std::uint16_t m_Port;
  std::string m_SubDir;
  bool m_SSL{false};
  std::string m_Name;
  std::string m_Description;
  std::filesystem::path s_CACertFilePath;
  std::filesystem::path s_CACertDirPath;
  std::map<std::string,Logbook> m_Logbooks;
};

}
