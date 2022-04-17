#pragma once

#include <voyx/Header.h>

#include <pocketfft/pocketfft_hdronly.h>

template<typename T>
class FFT
{

public:

  FFT(const size_t size) :
    size(size)
  {
    assert(size && !(size & (size - 1)));
  }

  // time domain vector size
  size_t tdsize() const
  {
    return size;
  }

  // frequency domain vector size
  size_t fdsize() const
  {
    return size / 2 + 1;
  }

  // normalized forward fft
  void fft(const std::vector<T>& signal, std::vector<std::complex<T>>& dft)
  {
    assert(signal.size() == tdsize());
    assert(dft.size() == fdsize());

    pocketfft::r2c(
      { signal.size() },
      { sizeof(T) },
      { sizeof(std::complex<T>) },
      0,
      true,
      signal.data(),
      dft.data(),
      T(1.0) / signal.size());
  }

  // backward fft
  void ifft(const std::vector<std::complex<T>>& dft, std::vector<T>& signal)
  {
    assert(signal.size() == tdsize());
    assert(dft.size() == fdsize());

    pocketfft::c2r(
      { signal.size() },
      { sizeof(std::complex<T>) },
      { sizeof(T) },
      0,
      false,
      dft.data(),
      signal.data(),
      T(1.0));
  }

private:

  const size_t size;

};
