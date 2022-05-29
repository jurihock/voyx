#pragma once

#include <voyx/Header.h>

namespace $$
{
  template<typename T>
  struct typeofvalue { typedef T type; };

  template<typename T>
  struct typeofvalue<std::complex<T>> { typedef T type; };

  struct real
  {
    template<typename T>
    T operator()(const T value) const { return value; };

    template<typename T>
    T operator()(const std::complex<T>& value) const { return std::real(value); };
  };

  struct imag
  {
    template<typename T>
    T operator()(const T value) const { return T(0); };

    template<typename T>
    T operator()(const std::complex<T>& value) const { return std::imag(value); };
  };

  struct abs
  {
    template<typename T>
    T operator()(const T value) const { return value; };

    template<typename T>
    T operator()(const std::complex<T>& value) const { return std::abs(value); };
  };

  struct arg
  {
    template<typename T>
    T operator()(const T value) const { return T(0); };

    template<typename T>
    T operator()(const std::complex<T>& value) const { return std::arg(value); };
  };
}
