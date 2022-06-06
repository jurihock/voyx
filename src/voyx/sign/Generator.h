#pragma once

#include <voyx/Header.h>

template<typename T>
class Generator
{

public:

  virtual ~Generator() {}

  virtual std::complex<T> operator()() = 0;

  virtual T cos()
  {
    return (*this)().real();
  }

  virtual T sin()
  {
    return (*this)().imag();
  }

  operator std::complex<T>()
  {
    return (*this)();
  }

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

};
