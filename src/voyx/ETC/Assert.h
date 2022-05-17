#pragma once

#include <voyx/Header.h>

namespace voyx
{
  struct assert
  {

    inline static void fail(const std::string& message)
    {
      throw std::runtime_error(message);
    }

    inline static void fail(const std::string& message, const std::string& altmessage)
    {
      if (message.empty())
      {
        throw std::runtime_error(altmessage);
      }
      else
      {
        throw std::runtime_error(message);
      }
    }

    inline static void ok(const bool x, const std::string& message = "")
    {
      if (x == true)
      {
        return;
      }

      fail(message, "Expected true, got false!");
    }

    inline static void nok(const bool x, const std::string& message = "")
    {
      if (x == false)
      {
        return;
      }

      fail(message, "Expected false, got true!");
    }

    inline static void null(const void* x, const std::string& message = "")
    {
      if (x == nullptr)
      {
        return;
      }

      fail(message, "Expected nullptr, got something!");
    }

    inline static void notnull(const void* x, const std::string& message = "")
    {
      if (x != nullptr)
      {
        return;
      }

      fail(message, "Expected something, got nullptr!");
    }

    template<class T>
    inline static void eq(const T& x, const T& y, const std::string& message = "")
    {
      if (x == y)
      {
        return;
      }

      fail(message, $("Expected {0}, got {1}!", y, x));
    }

    template<class T>
    inline static void neq(const T& x, const T& y, const std::string& message = "")
    {
      if (x != y)
      {
        return;
      }

      fail(message, $("Unexpected {0}!", x));
    }

  };
}
