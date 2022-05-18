#pragma once

#include <voyx/Header.h>

/**
 * Sliding DFT implementation according to [1].
 *
 * [1] Russell Bradford and Richard Dobson and John ffitch
 *     Sliding is Smoother than Jumping
 *     International Computer Music Conference 2005 (ICMC 2005)
 *     https://quod.lib.umich.edu/cgi/p/pod/dod-idx/sliding-is-smoother-than-jumping.pdf?c=icmc;idno=bbp2372.2005.086;format=pdf
 **/
template <class T>
class SDFT
{

public:

  SDFT(const size_t size) :
    size(size)
  {
    twiddles.resize(size);
    {
      const T pi = T(2) * std::acos(T(-1)) / size;

      for (size_t i = 0; i < size; ++i)
      {
        twiddles[i] = std::polar(T(1), pi * i);
      }
    }

    buffer.input.resize(size);
    buffer.output.resize(size + 2);
    buffer.cursor = 0;
  }

  void sdft(const T sample, voyx::vector<std::complex<T>> dft)
  {
    assert(dft.size() == size);

    const T scale = T(1) / size;
    const T delta = sample - std::exchange(buffer.input[buffer.cursor], sample);

    for (size_t i = 0, j = 1; i < size; ++i, ++j)
    {
      buffer.output[j] += delta;
      buffer.output[j] *= twiddles[i];
    }

    buffer.output[0] = buffer.output[size];
    buffer.output[size + 1] = buffer.output[1];

    for (size_t i = 0, j = 1; i < size; ++i, ++j)
    {
      dft[i] = window(buffer.output[j - 1],
                      buffer.output[j],
                      buffer.output[j + 1],
                      scale);
    }

    if (++buffer.cursor >= size)
    {
      buffer.cursor = 0;
    }
  }

  void sdft(const voyx::vector<T> samples, voyx::matrix<std::complex<T>> dfts)
  {
    assert(samples.size() == dfts.size());

    for (size_t i = 0; i < samples.size(); ++i)
    {
      sdft(samples[i], dfts[i]);
    }
  }

  T isdft(const voyx::vector<std::complex<T>> dft)
  {
    assert(dft.size() == size);

    T sample = T(0);

    for (size_t i = 0; i < size; ++i)
    {
      sample += dft[i].real() * (i % 2 ? -1 : +1);
    }

    return sample;
  }

  void isdft(const voyx::matrix<std::complex<T>> dfts, voyx::vector<T> samples)
  {
    assert(samples.size() == dfts.size());

    for (size_t i = 0; i < samples.size(); ++i)
    {
      samples[i] = isdft(dfts[i]);
    }
  }

private:

  const size_t size;

  std::vector<std::complex<T>> twiddles;

  struct
  {
    std::vector<T> input;
    std::vector<std::complex<T>> output;
    size_t cursor;
  }
  buffer;

  inline static std::complex<T> window(const std::complex<T>& left,
                                       const std::complex<T>& middle,
                                       const std::complex<T>& right,
                                       const T scale)
  {
    return T(0.25) * ((middle + middle) - (left + right)) * scale;
  }

};
