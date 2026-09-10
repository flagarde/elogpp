#pragma once
#include <string>
#include <string_view>

namespace cxx::elog
{
  std::string do_crypt(const std::string_view password);
}
