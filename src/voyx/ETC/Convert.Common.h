#pragma once

#include <voyx/Header.h>

namespace $$
{
  template<class T>
  std::constspan<T> constspan(std::span<T> value) { return value; }
}

namespace $$
{
  struct real
  {
    template<typename T>
    T operator()(const T value) const { return value; };

    template<typename T>
    T operator()(const std::complex<T>& value) const { return value.real(); };
  };

  struct imag
  {
    template<typename T>
    T operator()(const std::complex<T>& value) const { return value.imag(); };
  };
}

namespace $$
{
  template<typename T>
  struct typeofvalue { typedef T type; };

  template<typename T>
  struct typeofvalue<std::complex<T>> { typedef T type; };
}
