#pragma once

#include <voyx/Header.h>
#include <voyx/sign/Generator.h>

// TODO is amplitude drift fix required?
// https://dsp.stackexchange.com/a/1087

template<typename T>
class Oscillator : public Generator<T>
{

public:

  Oscillator() :
    samplerate(1),
    omega(1),
    phasor(1)
  {
  }

  Oscillator(const T frequency, const T samplerate) :
    samplerate(samplerate),
    omega(std::polar<T>(T(1), pi * frequency / samplerate)),
    phasor(1)
  {
  }

  Oscillator(const Oscillator<T>& other) :
    samplerate(other.samplerate),
    omega(other.omega),
    phasor(other.phasor)
  {
  }

  Oscillator<T>& operator=(const Oscillator<T>& other)
  {
    if (this != &other)
    {
      samplerate = other.samplerate;
      omega = other.omega;
      phasor = other.phasor;
    }

    return *this;
  }

  std::complex<T> operator()() override
  {
    return phasor *= omega;
  }

  std::complex<T> operator()(const T frequency)
  {
    omega = std::polar<T>(T(1), pi * frequency / samplerate);

    return phasor *= omega;
  }

  T cos()
  {
    return (*this)().real();
  }

  T cos(const T frequency)
  {
    return (*this)(frequency).real();
  }

  T sin()
  {
    return (*this)().imag();
  }

  T sin(const T frequency)
  {
    return (*this)(frequency).imag();
  }

private:

  const T pi = T(2) * std::acos(T(-1));

  T samplerate;

  std::complex<T> omega;
  std::complex<T> phasor;

};
