#pragma once

#include <voyx/Header.h>
#include <voyx/ETC/FFT.h>

template<typename T>
class Lifter
{

public:

  Lifter(const voyx_t quefrency, const voyx_t samplerate, const size_t framesize) :
    quefrency(static_cast<size_t>(quefrency * samplerate)),
    spectrum(framesize / 2 + 1),
    cepstrum(framesize),
    fft(framesize)
  {
  }

  std::vector<T> real(const std::constspan<T> dft)
  {
    std::vector<T> envelope(dft.size());
    real(dft, envelope);
    return envelope;
  }

  std::vector<T> real(const std::constspan<std::complex<T>> dft)
  {
    std::vector<T> envelope(dft.size());
    real(dft, envelope);
    return envelope;
  }

  std::vector<T> abs(const std::constspan<std::complex<T>> dft)
  {
    std::vector<T> envelope(dft.size());
    abs(dft, envelope);
    return envelope;
  }

  void real(const std::constspan<T> dft, std::span<T> envelope)
  {
    assert(dft.size() == envelope.size());

    for (size_t i = 0; i < spectrum.size(); ++i)
    {
      spectrum[i] = std::log10(dft[i]); // as is
    }

    fft.ifft(spectrum, cepstrum);
    lowpass(cepstrum, quefrency);
    fft.fft(cepstrum, spectrum);

    for (size_t i = 0; i < spectrum.size(); ++i)
    {
      envelope[i] = std::pow(T(10), spectrum[i].real());
    }
  }

  void real(const std::constspan<std::complex<T>> dft, std::span<T> envelope)
  {
    assert(dft.size() == envelope.size());

    for (size_t i = 0; i < spectrum.size(); ++i)
    {
      spectrum[i] = std::log10(std::real(dft[i])); // real
    }

    fft.ifft(spectrum, cepstrum);
    lowpass(cepstrum, quefrency);
    fft.fft(cepstrum, spectrum);

    for (size_t i = 0; i < spectrum.size(); ++i)
    {
      envelope[i] = std::pow(T(10), spectrum[i].real());
    }
  }

  void abs(const std::constspan<std::complex<T>> dft, std::span<T> envelope)
  {
    assert(dft.size() == envelope.size());

    for (size_t i = 0; i < spectrum.size(); ++i)
    {
      spectrum[i] = std::log10(std::abs(dft[i])); // abs
    }

    fft.ifft(spectrum, cepstrum);
    lowpass(cepstrum, quefrency);
    fft.fft(cepstrum, spectrum);

    for (size_t i = 0; i < spectrum.size(); ++i)
    {
      envelope[i] = std::pow(T(10), spectrum[i].real());
    }
  }

private:

  const size_t quefrency;

  std::vector<std::complex<T>> spectrum;
  std::vector<T> cepstrum;

  FFT<T> fft;

  static void lowpass(std::vector<T>& cepstrum, const size_t quefrency)
  {
    for (size_t i = 1; i < std::min(quefrency, cepstrum.size()); ++i)
    {
      cepstrum[i] *= 2;
    }

    for (size_t i = quefrency + 1; i < cepstrum.size(); ++i)
    {
      cepstrum[i] = 0;
    }
  }

};
