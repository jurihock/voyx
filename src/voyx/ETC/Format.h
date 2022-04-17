#pragma once

#include <voyx/Header.h>

#include <fmt/format.h>

template <typename... Args>
inline static std::string $(const char* string, Args&&... args)
{
  return fmt::format(string, args...);
}

template <typename... Args>
inline static std::string $(const std::string& string, Args&&... args)
{
  return fmt::format(string, args...);
}
