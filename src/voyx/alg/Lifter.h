#pragma once

#include <voyx/Header.h>
#include <voyx/alg/FFT.h>

template<typename T>
class Lifter
{

public:

  Lifter(const voyx_t quefrency, const voyx_t samplerate, const size_t framesize) :
    samplerate(samplerate),
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

  void lowpass(const voyx::vector<T> dft, voyx::vector<T> envelope)
  {
    voyxassert(dft.size() == envelope.size());

    for (size_t i = 1; i < dft.size() - 1; ++i)
    {
      const T value = dft[i];

      spectrum[i] = value ? std::log10(value) : -120;
    }

    fft.ifft(spectrum, cepstrum);
    lowpass(cepstrum, quefrency);
    fft.fft(cepstrum, spectrum);

    for (size_t i = 1; i < dft.size() - 1; ++i)
    {
      envelope[i] = std::pow(T(10), spectrum[i].real());
    }
  }

  template<typename value_getter_t>
  std::vector<T> lowpass(const voyx::vector<std::complex<T>> dft)
  {
    std::vector<T> envelope(dft.size());
    lowpass<value_getter_t>(dft, envelope);
    return envelope;
  }

  template<typename value_getter_t>
  void lowpass(const voyx::vector<std::complex<T>> dft, voyx::vector<T> envelope)
  {
    const value_getter_t getvalue;

    voyxassert(dft.size() == envelope.size());

    for (size_t i = 1; i < dft.size() - 1; ++i)
    {
      const T value = getvalue(dft[i]);

      spectrum[i] = value ? std::log10(value) : -120;
    }

    fft.ifft(spectrum, cepstrum);
    lowpass(cepstrum, quefrency);
    fft.fft(cepstrum, spectrum);

    for (size_t i = 1; i < dft.size() - 1; ++i)
    {
      envelope[i] = std::pow(T(10), spectrum[i].real());
    }
  }

  template<typename value_getter_t>
  void lowpass(const voyx::vector<std::complex<T>> dft, voyx::vector<T> envelope, voyx::vector<T> logcepstrum)
  {
    const value_getter_t getvalue;

    voyxassert(dft.size() == envelope.size());
    voyxassert(logcepstrum.size() == cepstrum.size());

    for (size_t i = 1; i < dft.size() - 1; ++i)
    {
      const T value = getvalue(dft[i]);

      spectrum[i] = value ? std::log10(value) : -120;
    }

    fft.ifft(spectrum, cepstrum);
    logcepstrum = cepstrum;
    lowpass(cepstrum, quefrency);
    fft.fft(cepstrum, spectrum);

    for (size_t i = 1; i < dft.size() - 1; ++i)
    {
      envelope[i] = std::pow(T(10), spectrum[i].real());
    }
  }

  template<typename value_getter_setter_t>
  void divide(voyx::vector<std::complex<T>> dft, const voyx::vector<T> envelope) const
  {
    const value_getter_setter_t value;

    for (size_t i = 0; i < dft.size(); ++i)
    {
      const bool ok = std::isnormal(envelope[i]);

      value(dft[i], ok ? value(dft[i]) / envelope[i] : 0);
    }
  }

  template<typename value_getter_setter_t>
  void multiply(voyx::vector<std::complex<T>> dft, const voyx::vector<T> envelope) const
  {
    const value_getter_setter_t value;

    for (size_t i = 0; i < dft.size(); ++i)
    {
      const bool ok = std::isnormal(envelope[i]);

      value(dft[i], ok ? value(dft[i]) * envelope[i] : 0);
    }
  }

  voyx_t f0(const voyx::vector<T> logcepstrum, std::pair<voyx_t, voyx_t> roi) const
  {
    const size_t nmin = size_t(0);
    const size_t nmax = logcepstrum.size();

    const size_t qmin = static_cast<size_t>(samplerate / roi.first);
    const size_t qmax = static_cast<size_t>(samplerate / roi.second);

    const voyx_t imin = std::clamp(std::min(qmin, qmax), nmin, nmax);
    const voyx_t imax = std::clamp(std::max(qmin, qmax), nmin, nmax);

    voyxassert(imin < imax);

    T value = std::numeric_limits<T>::lowest();
    ptrdiff_t index = -1;

    for (size_t i = imin; i < imax; ++i)
    {
      if (logcepstrum[i] > value)
      {
        value = logcepstrum[i];
        index = i;
      }
    }

    return index ? samplerate / index : 0;
  }

private:

  const voyx_t samplerate;
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
