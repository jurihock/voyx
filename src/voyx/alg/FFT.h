#pragma once

#include <voyx/Header.h>
#include <voyx/ALG/Window.h>

#include <pocketfft_hdronly.h>

template<typename T>
class FFT
{

public:

  FFT(const size_t size) :
    size(size)
  {
    voyxassert(size && !(size & (size - 1)));
  }

  /**
   * Time domain real vector size.
   **/
  size_t tdsize() const
  {
    return size;
  }

  /**
   * Frequency domain complex vector size.
   **/
  size_t fdsize() const
  {
    return size / 2 + 1;
  }

  void fft(const voyx::vector<T> samples, voyx::vector<std::complex<T>> dft) const
  {
    voyxassert(samples.size() == tdsize());
    voyxassert(dft.size() == fdsize());

    pocketfft::r2c(
      { samples.size() },
      { sizeof(T) },
      { sizeof(std::complex<T>) },
      0,
      true,
      samples.data(),
      dft.data(),
      T(1) / samples.size());
  }

  void fft(const voyx::matrix<T> samples, voyx::matrix<std::complex<T>> dfts) const
  {
    voyxassert(samples.size() == dfts.size());
    voyxassert(samples.stride() == tdsize());
    voyxassert(dfts.stride() == fdsize());

    const ptrdiff_t X = samples.stride() * sizeof(T);
    const ptrdiff_t Y = dfts.stride() * sizeof(std::complex<T>);

    pocketfft::r2c(
      { samples.stride(), samples.size() },
      { sizeof(T), X },
      { sizeof(std::complex<T>), Y },
      0,
      true,
      samples.data(),
      dfts.data(),
      T(1) / samples.stride());
  }

  void ifft(const voyx::vector<std::complex<T>> dft, voyx::vector<T> samples) const
  {
    voyxassert(samples.size() == tdsize());
    voyxassert(dft.size() == fdsize());

    pocketfft::c2r(
      { samples.size() },
      { sizeof(std::complex<T>) },
      { sizeof(T) },
      0,
      false,
      dft.data(),
      samples.data(),
      T(1));
  }

  void ifft(const voyx::matrix<std::complex<T>> dfts, voyx::matrix<T> samples) const
  {
    voyxassert(samples.size() == dfts.size());
    voyxassert(samples.stride() == tdsize());
    voyxassert(dfts.stride() == fdsize());

    const ptrdiff_t X = dfts.stride() * sizeof(std::complex<T>);
    const ptrdiff_t Y = samples.stride() * sizeof(T);

    pocketfft::c2r(
      { samples.stride(), samples.size() },
      { sizeof(std::complex<T>), X },
      { sizeof(T), Y },
      0,
      false,
      dfts.data(),
      samples.data(),
      T(1));
  }

  std::vector<std::complex<T>> fft(const voyx::vector<T> samples) const
  {
    voyxassert(samples.size() == tdsize());

    const std::vector<T> window = Window<T>(tdsize());

    return fft(samples, window);
  }

  std::vector<std::complex<T>> fft(const voyx::vector<T> samples, const voyx::vector<T> window) const
  {
    voyxassert(samples.size() == tdsize());
    voyxassert(window.size() == tdsize());

    std::vector<T> product(tdsize());
    std::vector<std::complex<T>> dft(fdsize());

    for (size_t i = 0; i < tdsize(); ++i)
    {
      product[i] = samples[i] * window[i];
    }

    fft(product, dft);

    return dft;
  }

  std::vector<T> ifft(const voyx::vector<std::complex<T>> dft) const
  {
    voyxassert(dft.size() == fdsize());

    std::vector<T> samples(tdsize());

    ifft(dft, samples);

    return samples;
  }

private:

  const size_t size;

};
