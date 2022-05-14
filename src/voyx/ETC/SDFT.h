#pragma once

#include <voyx/Header.h>

template <class T>
class SDFT
{

public:

  SDFT(const size_t size) :
    size(size)
  {
    twiddles.forward.resize(size);
    {
      const T pi = T(2) * std::acos(T(-1)) / size;

      for (size_t i = 0; i < size; ++i)
      {
        twiddles.forward[i] = std::polar(T(1), pi * i);
      }
    }

    twiddles.backward.resize(size);
    {
      const T pi = T(1) * std::acos(T(-1));

      for (size_t i = 0; i < size; ++i)
      {
        twiddles.backward[i] = std::polar(T(1), pi * i);
      }
    }

    buffer.input.resize(size);
    buffer.output.resize(size);
    buffer.cursor = 0;
  }

  void sdft(const T sample, const std::span<std::complex<T>> dft)
  {
    assert(dft.size() == size);

    const T bias = buffer.input[buffer.cursor];

    buffer.input[buffer.cursor] = sample;

    for (size_t i = 0; i < size; ++i)
    {
      buffer.output[i] = twiddles.forward[i] * (buffer.output[i] + sample - bias);
    }

    dft[0] = window(buffer.output[size - 1],
                    buffer.output[0],
                    buffer.output[1]);

    for (size_t i = 1; i < size - 1; ++i)
    {
      dft[i] = window(buffer.output[i - 1],
                      buffer.output[i],
                      buffer.output[i + 1]);
    }

    dft[size - 1] = window(buffer.output[size - 2],
                           buffer.output[size - 1],
                           buffer.output[0]);

    const T scale = T(1) / size;

    for (size_t i = 0; i < size; ++i)
    {
      dft[i] *= scale;
    }

    if (++buffer.cursor >= size)
    {
      buffer.cursor = 0;
    }
  }

  void sdft(const std::vector<T>& samples, const std::matrix<std::complex<T>>& dfts)
  {
    assert(samples.size() == dfts.size());

    for (size_t i = 0; i < samples.size(); ++i)
    {
      sdft(samples[i], dfts[i]);
    }
  }

  T isdft(const std::span<std::complex<T>> dft)
  {
    assert(dft.size() == size);

    T sample = T(0);

    for (size_t i = 0; i < size; ++i)
    {
      sample += (dft[i] * twiddles.backward[i]).real();
    }

    return sample;
  }

  void isdft(const std::matrix<std::complex<T>>& dfts, std::vector<T>& samples)
  {
    assert(samples.size() == dfts.size());

    for (size_t i = 0; i < samples.size(); ++i)
    {
      samples[i] = isdft(dfts[i]);
    }
  }

private:

  const size_t size;

  struct
  {
    std::vector<std::complex<T>> forward;
    std::vector<std::complex<T>> backward;
  }
  twiddles;

  struct
  {
    std::vector<T> input;
    std::vector<std::complex<T>> output;
    size_t cursor;
  }
  buffer;

  inline static std::complex<T> window(const std::complex<T>& left,
                                       const std::complex<T>& middle,
                                       const std::complex<T>& right)
  {
    return T(0.5) * middle - T(0.25) * (left + right);
  }

};
