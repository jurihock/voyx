#pragma once

#include <voyx/Header.h>

template <class T>
class SDFT
{

public:

  SDFT(const size_t size) :
    size(size),
    twiddles(size),
    sdft_input_buffer(size),
    sdft_output_buffer(size),
    sdft_input_cursor(0)
  {
    const T pi = T(2) * std::acos(T(-1)) / size;

    for (size_t i = 0; i < size; ++i)
    {
      twiddles[i] = std::polar(T(1), pi * i);
    }
  }

  void sdft(const T sample, const std::span<std::complex<T>> dft)
  {
    assert(dft.size() == size);

    const T bias = sdft_input_buffer[sdft_input_cursor];

    sdft_input_buffer[sdft_input_cursor] = sample;

    for (size_t i = 0; i < size; ++i)
    {
      sdft_output_buffer[i] = twiddles[i] * (sdft_output_buffer[i] + sample - bias);
    }

    dft[0] = window(sdft_output_buffer[0],
                    sdft_output_buffer[size - 1],
                    sdft_output_buffer[1]);

    for (size_t i = 1; i < size - 1; ++i)
    {
      dft[i] = window(sdft_output_buffer[i],
                      sdft_output_buffer[i - 1],
                      sdft_output_buffer[i + 1]);
    }

    dft[size - 1] = window(sdft_output_buffer[size - 1],
                           sdft_output_buffer[size - 2],
                           sdft_output_buffer[0]);

    if (++sdft_input_cursor >= size)
    {
      sdft_input_cursor = 0;
    }
  }

  void sdft(const std::vector<T>& samples, const std::matrix<std::complex<T>>& dfts)
  {
    assert(samples.size() == dfts.size());

    for (size_t i = 0; i < samples.size(); ++i)
    {
      sdft(samples[i], dfts[i]);

      const T scale = T(1) / samples.size(); // TODO

      for (size_t j = 0; j < dfts[i].size(); ++j)
      {
        dfts[i][j] *= scale;
      }
    }
  }

private:

  const size_t size;

  std::vector<std::complex<T>> twiddles;
  std::vector<T> sdft_input_buffer;
  std::vector<std::complex<T>> sdft_output_buffer;

  size_t sdft_input_cursor;

  inline static std::complex<T> window(const std::complex<T>& a, const std::complex<T>& b, const std::complex<T>& c)
  {
    return T(0.5) * a - T(0.25) * (b + c);
  }

};
