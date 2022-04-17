#pragma once

#include <voyx/Header.h>

template<typename T>
class Window
{

public:

  Window(const size_t size) :
    size(size)
  {
  }

  operator std::vector<T>() const
  {
    return hann();
  }

  std::vector<T> hann() const
  {
    std::vector<T> window(size);

    std::iota(window.begin(), window.end(), T(0.0));

    std::transform(window.begin(), window.end(), window.begin(),
      [&](T value) { return T(0.5) - T(0.5) * std::cos(PI2 * value / window.size()); });

    return window;
  }

private:

  const T PI2 = T(2.0) * std::acos(T(-1.0));

  const size_t size;

};
