#pragma once

#include <voyx/Header.h>

// TODO is amplitude drift fix required?
// https://dsp.stackexchange.com/a/1087

template<class T>
class Oscillator
{

public:

  Oscillator(const T frequency, const T samplerate) :
    samplerate(samplerate),
    omega(std::polar<T>(T(1), pi * frequency / samplerate)),
    phasor(-1)
  {
  }

  T operator()()
  {
    return (phasor *= omega).real();
  }

  T operator()(const T frequency)
  {
    omega = std::polar<T>(T(1), pi * frequency / samplerate);

    return (phasor *= omega).real();
  }

private:

  const T pi = T(2) * std::acos(T(-1));

  const T samplerate;

  std::complex<T> omega;
  std::complex<T> phasor;

};
