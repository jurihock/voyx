#pragma once

#include <voyx/Header.h>

template<class T>
class Oscillator
{

public:

  Oscillator(const T frequency, const T samplerate) :
    samplerate(samplerate),
    omega(std::polar<T>(T(1), pi * frequency / samplerate)),
    phasor(1)
  {
  }

  T operator()()
  {
    return (phasor *= omega).imag();
  }

  T operator()(const T frequency)
  {
    omega = std::polar<T>(T(1), pi * frequency / samplerate);

    return (phasor *= omega).imag();
  }

private:

  const T pi = T(2) * std::acos(T(-1));

  const T samplerate;

  std::complex<T> omega;
  std::complex<T> phasor;

};
