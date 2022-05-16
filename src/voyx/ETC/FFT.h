#pragma once

#include <voyx/Header.h>
#include <voyx/ETC/Window.h>

#include <pocketfft_hdronly.h>

template<typename T>
class FFT
{

public:

  FFT(const size_t size) :
    size(size)
  {
    assert(size && !(size & (size - 1)));
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

  void fft(const voyx::vector<T> signal, voyx::vector<std::complex<T>> dft) const
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
      T(1) / signal.size());
  }

  void fft(const voyx::matrix<T> signals, voyx::matrix<std::complex<T>> dfts) const
  {
    assert(signals.size() == dfts.size());
    assert(signals.stride() == tdsize());
    assert(dfts.stride() == fdsize());

    const ptrdiff_t X = signals.stride() * sizeof(T);
    const ptrdiff_t Y = dfts.stride() * sizeof(std::complex<T>);

    pocketfft::r2c(
      { signals.stride(), signals.size() },
      { sizeof(T), X },
      { sizeof(std::complex<T>), Y },
      0,
      true,
      signals.data(),
      dfts.data(),
      T(1) / signals.stride());
  }

  void ifft(const voyx::vector<std::complex<T>> dft, voyx::vector<T> signal) const
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
      T(1));
  }

  void ifft(const voyx::matrix<std::complex<T>> dfts, voyx::matrix<T> signals) const
  {
    assert(signals.size() == dfts.size());
    assert(signals.stride() == tdsize());
    assert(dfts.stride() == fdsize());

    const ptrdiff_t X = dfts.stride() * sizeof(std::complex<T>);
    const ptrdiff_t Y = signals.stride() * sizeof(T);

    pocketfft::c2r(
      { signals.stride(), signals.size() },
      { sizeof(std::complex<T>), X },
      { sizeof(T), Y },
      0,
      false,
      dfts.data(),
      signals.data(),
      T(1));
  }

  std::vector<std::complex<T>> fft(const voyx::vector<T> signal) const
  {
    assert(signal.size() == tdsize());

    const std::vector<T> window = Window<T>(tdsize());

    return fft(signal, window);
  }

  std::vector<std::complex<T>> fft(const voyx::vector<T> signal, const voyx::vector<T> window) const
  {
    assert(signal.size() == tdsize());
    assert(window.size() == tdsize());

    std::vector<T> sigwin(tdsize());
    std::vector<std::complex<T>> dft(fdsize());

    for (size_t i = 0; i < tdsize(); ++i)
    {
      sigwin[i] = signal[i] * window[i];
    }

    fft(sigwin, dft);

    return dft;
  }

  std::vector<T> ifft(const voyx::vector<std::complex<T>> dft) const
  {
    assert(dft.size() == fdsize());

    std::vector<T> signal(tdsize());

    ifft(dft, signal);

    return signal;
  }

private:

  const size_t size;

};
