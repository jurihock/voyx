#pragma once

#include <voyx/Header.h>

template<typename T>
class Generator
{

public:

  virtual ~Generator() {}

  virtual std::complex<T> operator()() = 0;

  operator std::complex<T>()
  {
    return (*this)();
  }

  operator T()
  {
    return (*this)().real();
  }

  T operator++(int)
  {
    return (*this)().real();
  }

  T operator--(int)
  {
    return (*this)().imag();
  }

  // ADDITION

  template<typename V>
  friend std::complex<V> operator+(Generator<T>& g, const V v)
  {
    return v + g();
  }

  template<typename V>
  friend std::complex<V> operator+(Generator<T>& g, const std::complex<V>& v)
  {
    return v + g();
  }

  template<typename V>
  friend std::complex<V> operator+(const V v, Generator<T>& g)
  {
    return v + g();
  }

  template<typename V>
  friend std::complex<V> operator+(const std::complex<V>& v, Generator<T>& g)
  {
    return v + g();
  }

  template<typename V>
  friend std::complex<V>& operator+=(std::complex<V>& v, Generator<T>& g)
  {
    return v += g();
  }

  // SUBTRACTION

  template<typename V>
  friend std::complex<V> operator-(Generator<T>& g, const V v)
  {
    return v - g();
  }

  template<typename V>
  friend std::complex<V> operator-(Generator<T>& g, const std::complex<V>& v)
  {
    return v - g();
  }

  template<typename V>
  friend std::complex<V> operator-(const V v, Generator<T>& g)
  {
    return v - g();
  }

  template<typename V>
  friend std::complex<V> operator-(const std::complex<V>& v, Generator<T>& g)
  {
    return v - g();
  }

  template<typename V>
  friend std::complex<V>& operator-=(std::complex<V>& v, Generator<T>& g)
  {
    return v -= g();
  }

  // MULTIPLICATION

  template<typename V>
  friend std::complex<V> operator*(Generator<T>& g, const V v)
  {
    return v * g();
  }

  template<typename V>
  friend std::complex<V> operator*(Generator<T>& g, const std::complex<V>& v)
  {
    return v * g();
  }

  template<typename V>
  friend std::complex<V> operator*(const V v, Generator<T>& g)
  {
    return v * g();
  }

  template<typename V>
  friend std::complex<V> operator*(const std::complex<V>& v, Generator<T>& g)
  {
    return v * g();
  }

  template<typename V>
  friend std::complex<V>& operator*=(std::complex<V>& v, Generator<T>& g)
  {
    return v *= g();
  }

  // DIVISION

  template<typename V>
  friend std::complex<V> operator/(Generator<T>& g, const V v)
  {
    return v / g();
  }

  template<typename V>
  friend std::complex<V> operator/(Generator<T>& g, const std::complex<V>& v)
  {
    return v / g();
  }

  template<typename V>
  friend std::complex<V> operator/(const V v, Generator<T>& g)
  {
    return v / g();
  }

  template<typename V>
  friend std::complex<V> operator/(const std::complex<V>& v, Generator<T>& g)
  {
    return v / g();
  }

  template<typename V>
  friend std::complex<V>& operator/=(std::complex<V>& v, Generator<T>& g)
  {
    return v /= g();
  }

};
