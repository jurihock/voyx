#pragma once

#include <voyx/Header.h>

template<typename T>
class Window
{

public:

  Window(const size_t framesize) :
    framesize(framesize)
  {
  }

  operator std::vector<T>() const
  {
    return hann();
  }

  std::vector<T> hann() const
  {
    std::vector<T> window(framesize);

    const T pi = T(2) * std::acos(T(-1)) / framesize;

    for (size_t i = 0; i < framesize; ++i)
    {
      window[i] = T(0.5) - T(0.5) * std::cos(pi * i);
    }

    return window;
  }

private:

  const size_t framesize;

};
