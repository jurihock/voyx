#pragma once

#include <voyx/Header.h>

#include <fmt/format.h>

template <typename... Args>
std::string $(fmt::format_string<Args...> string, Args&&... args)
{
  return fmt::format(string, std::forward<Args>(args)...);
}
