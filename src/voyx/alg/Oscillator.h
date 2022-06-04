#pragma once

#include <voyx/Header.h>

// TODO is amplitude drift fix required?
// https://dsp.stackexchange.com/a/1087

template<typename T>
class Oscillator
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

  operator std::complex<T>()
  {
    return phasor *= omega;
  }

  std::complex<T> operator()()
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

  template<typename V>
  friend std::complex<V> operator*(Oscillator<T>& o, const V v)
  {
    return v * o();
  }

  template<typename V>
  friend std::complex<V> operator*(Oscillator<T>& o, const std::complex<V>& v)
  {
    return v * o();
  }

  template<typename V>
  friend std::complex<V> operator*(const V v, Oscillator<T>& o)
  {
    return v * o();
  }

  template<typename V>
  friend std::complex<V> operator*(const std::complex<V>& v, Oscillator<T>& o)
  {
    return v * o();
  }

private:

  const T pi = T(2) * std::acos(T(-1));

  T samplerate;

  std::complex<T> omega;
  std::complex<T> phasor;

};
