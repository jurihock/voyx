#pragma once

#include <voyx/Header.h>
#include <voyx/alg/Oscillator.h>

template<typename T>
class Wobbulator
{

public:

  Wobbulator()
  {
  }

  Wobbulator(std::pair<T, T> frequencies, T duration, const T samplerate) :
    slope((frequencies.first - frequencies.second) / T(2)),
    intercept((frequencies.first + frequencies.second) / T(2)),
    lfo(T(1) / duration, samplerate),
    osc(frequencies.first, samplerate)
  {
  }

  Wobbulator(const Wobbulator<T>& other) :
    slope(other.slope),
    intercept(other.intercept),
    lfo(other.lfo),
    osc(other.osc)
  {
  }

  Wobbulator<T>& operator=(const Wobbulator<T>& other)
  {
    if (this != &other)
    {
      slope = other.slope;
      intercept = other.intercept;
      lfo = other.lfo;
      osc = other.osc;
    }

    return *this;
  }

  operator std::complex<T>()
  {
    return osc(lfo.cos() * slope + intercept);
  }

  std::complex<T> operator()()
  {
    return osc(lfo.cos() * slope + intercept);
  }

  T cos()
  {
    return (*this)().real();
  }

  T sin()
  {
    return (*this)().imag();
  }

  template<typename V>
  friend std::complex<V> operator+(Wobbulator<T>& o, const V v)
  {
    return v + o();
  }

  template<typename V>
  friend std::complex<V> operator+(Wobbulator<T>& o, const std::complex<V>& v)
  {
    return v + o();
  }

  template<typename V>
  friend std::complex<V> operator+(const V v, Wobbulator<T>& o)
  {
    return v + o();
  }

  template<typename V>
  friend std::complex<V> operator+(const std::complex<V>& v, Wobbulator<T>& o)
  {
    return v + o();
  }

  template<typename V>
  friend std::complex<V>& operator+=(std::complex<V>& v, Wobbulator<T>& o)
  {
    return v += o();
  }

  template<typename V>
  friend std::complex<V> operator*(Wobbulator<T>& o, const V v)
  {
    return v * o();
  }

  template<typename V>
  friend std::complex<V> operator*(Wobbulator<T>& o, const std::complex<V>& v)
  {
    return v * o();
  }

  template<typename V>
  friend std::complex<V> operator*(const V v, Wobbulator<T>& o)
  {
    return v * o();
  }

  template<typename V>
  friend std::complex<V> operator*(const std::complex<V>& v, Wobbulator<T>& o)
  {
    return v * o();
  }

  template<typename V>
  friend std::complex<V>& operator*=(std::complex<V>& v, Wobbulator<T>& o)
  {
    return v *= o();
  }

private:

  T slope;
  T intercept;

  Oscillator<T> lfo;
  Oscillator<T> osc;

};
