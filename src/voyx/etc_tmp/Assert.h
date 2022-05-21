#pragma once

#include <voyx/Header.h>

#define voyxassert(condition) \
  voyx::assert::check(        \
    (condition),              \
    (#condition),             \
    __FILE__,                 \
    __LINE__);

namespace voyx
{
  struct assert
  {
    static void fail(const std::string_view message, const std::string_view file, const int line)
    {
      std::ostringstream error;

      error << message << " "
            << "in "   << std::filesystem::path(file).filename().string() << " "
            << "at "   << line;

      throw std::runtime_error(error.str());
    }

    static void check(const bool condition, const std::string_view message, const std::string_view file, const int line)
    {
      if (!condition)
      {
        fail(message, file, line);
      }
    }
  };
}
