#pragma once

#include <voyx/Header.h>

template<typename T>
class SRC
{

public:

  SRC() :
    samplerates()
  {
  }

  SRC(const std::pair<voyx_t, voyx_t>& samplerates) :
    samplerates(samplerates)
  {
  }

  SRC(const SRC& other) :
    samplerates(other.samplerates)
  {
  }

  SRC& operator=(const std::pair<voyx_t, voyx_t>& samplerates)
  {
    this->samplerates = samplerates;

    return *this;
  }

  SRC& operator=(const SRC& other)
  {
    if (this != &other)
    {
      this->samplerates = other.samplerates;
    }

    return *this;
  }

  voyx_t quotient() const
  {
    return samplerates.second / samplerates.first;
  }

  void operator()(const voyx::vector<T> src, voyx::vector<T> dst) const
  {
    voyxassert(dst.size() == static_cast<size_t>(src.size() * quotient()));

    if (samplerates.second == samplerates.first)
    {
      dst = src;
    }
    else if (samplerates.second == samplerates.first * 4)
    {
      for (size_t i = 0, j = 0; i < src.size(); i+=1, j+=4)
      {
        dst[j + 0] = src[i];
        dst[j + 1] = src[i];
        dst[j + 2] = src[i];
        dst[j + 3] = src[i];
      }
    }
    else if (samplerates.second == samplerates.first * 3)
    {
      for (size_t i = 0, j = 0; i < src.size(); i+=1, j+=3)
      {
        dst[j + 0] = src[i];
        dst[j + 1] = src[i];
        dst[j + 2] = src[i];
      }
    }
    else if (samplerates.second == samplerates.first * 2)
    {
      for (size_t i = 0, j = 0; i < src.size(); i+=1, j+=2)
      {
        dst[j + 0] = src[i];
        dst[j + 1] = src[i];
      }
    }
    else if (samplerates.second * 2 == samplerates.first)
    {
      for (size_t i = 0, j = 0; i < src.size(); i+=2, j+=1)
      {
        dst[j] = src[i];
      }
    }
    else if (samplerates.second * 3 == samplerates.first)
    {
      for (size_t i = 0, j = 0; i < src.size(); i+=3, j+=1)
      {
        dst[j] = src[i];
      }
    }
    else if (samplerates.second * 4 == samplerates.first)
    {
      for (size_t i = 0, j = 0; i < src.size(); i+=4, j+=1)
      {
        dst[j] = src[i];
      }
    }
    else
    {
      std::ostringstream error;

      error
        << "Unsupported sample rate conversion "
        << "from " << samplerates.first << " Hz "
        << "to " << samplerates.second << " Hz!";

      throw std::runtime_error(error.str());
    }
  }

private:

  std::pair<voyx_t, voyx_t> samplerates;

};
