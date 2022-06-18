#pragma once

#include <voyx/Header.h>
#include <voyx/alg/FFT.h>

template<typename T>
class Lifter
{

public:

  Lifter(const voyx_t quefrency, const voyx_t samplerate, const size_t framesize) :
    quefrency(static_cast<size_t>(quefrency * samplerate)),
    fft(framesize),
    spectrum(fft.dftsize()),
    cepstrum(fft.framesize())
  {
  }

  std::vector<T> lowpass(const voyx::vector<T>& dft)
  {
    std::vector<T> envelope(dft.size());
    lowpass(dft, envelope);
    return envelope;
  }

  void lowpass(const voyx::vector<T>& dft, std::span<T> envelope)
  {
    voyxassert(dft.size() == envelope.size());

    for (size_t i = 0; i < dft.size(); ++i)
    {
      spectrum[i] = std::log10(dft[i]);
    }

    fft.ifft(spectrum, cepstrum);
    lowpass(cepstrum, quefrency);
    fft.fft(cepstrum, spectrum);

    for (size_t i = 0; i < dft.size(); ++i)
    {
      envelope[i] = std::pow(T(10), spectrum[i].real());
    }
  }

  template<typename value_getter_t>
  std::vector<T> lowpass(const voyx::vector<std::complex<T>>& dft)
  {
    std::vector<T> envelope(dft.size());
    lowpass<value_getter_t>(dft, envelope);
    return envelope;
  }

  template<typename value_getter_t>
  void lowpass(const voyx::vector<std::complex<T>>& dft, std::span<T> envelope)
  {
    const value_getter_t getvalue;

    voyxassert(dft.size() == envelope.size());

    for (size_t i = 0; i < dft.size(); ++i)
    {
      spectrum[i] = std::log10(getvalue(dft[i]));
    }

    fft.ifft(spectrum, cepstrum);
    lowpass(cepstrum, quefrency);
    fft.fft(cepstrum, spectrum);

    for (size_t i = 0; i < dft.size(); ++i)
    {
      envelope[i] = std::pow(T(10), spectrum[i].real());
    }
  }

private:

  const size_t quefrency;

  const FFT<T> fft;

  std::vector<std::complex<T>> spectrum;
  std::vector<T> cepstrum;

  static void lowpass(std::span<T> cepstrum, const size_t quefrency)
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
