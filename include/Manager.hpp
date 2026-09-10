#pragma once
#include "Entry.hpp"

namespace cxx::elog
{

class Manager
{
public:
  Manager()=default;
  void setVerbosity(const bool);
  bool isVerbose();
  Entry createEntry();
private:
  bool m_Verbose{false};
};

}
