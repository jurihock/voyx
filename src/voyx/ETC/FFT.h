#pragma once

#include <voyx/Header.h>
#include <voyx/ETC/Window.h>

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

  /**
   * Time domain real vector size.
   * */
  size_t tdsize() const
  {
    return size;
  }

  /**
   * Frequency domain complex vector size.
   * */
  size_t fdsize() const
  {
    return size / 2 + 1;
  }

  void fft(const std::span<const T> signal, const std::span<std::complex<T>> dft) const
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

  void fft(const std::vector<std::span<T>>& signals, const std::vector<std::span<std::complex<T>>>& dfts) const
  {
    assert(signals.size() == dfts.size());
    assert(signals.front().size() == tdsize());
    assert(dfts.front().size() == fdsize());

    const ptrdiff_t X = signals.front().size() * sizeof(T);
    const ptrdiff_t Y = dfts.front().size() * sizeof(std::complex<T>);

    pocketfft::r2c(
      { signals.front().size(), signals.size() },
      { sizeof(T), X },
      { sizeof(std::complex<T>), Y },
      0,
      true,
      signals.front().data(),
      dfts.front().data(),
      T(1.0) / signals.front().size());
  }

  void ifft(const std::span<const std::complex<T>> dft, const std::span<T> signal) const
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

  void ifft(const std::vector<std::span<std::complex<T>>>& dfts, const std::vector<std::span<T>>& signals) const
  {
    assert(signals.size() == dfts.size());
    assert(signals.front().size() == tdsize());
    assert(dfts.front().size() == fdsize());

    const ptrdiff_t X = dfts.front().size() * sizeof(std::complex<T>);
    const ptrdiff_t Y = signals.front().size() * sizeof(T);

    pocketfft::c2r(
      { signals.front().size(), signals.size() },
      { sizeof(std::complex<T>), X },
      { sizeof(T), Y },
      0,
      false,
      dfts.front().data(),
      signals.front().data(),
      T(1.0));
  }

  std::vector<std::complex<T>> fft(const std::span<const T> signal) const
  {
    assert(signal.size() == tdsize());

    const std::vector<T> window = Window<T>(tdsize());

    return fft(signal, window);
  }

  std::vector<std::complex<T>> fft(const std::span<const T> signal, const std::span<const T> window) const
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

  std::vector<T> ifft(const std::span<const std::complex<T>> dft) const
  {
    assert(dft.size() == fdsize());

    std::vector<T> signal(tdsize());

    ifft(dft, signal);

    return signal;
  }

private:

  const size_t size;

};
