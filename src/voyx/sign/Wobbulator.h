#pragma once

#include <voyx/Header.h>
#include <voyx/sign/Generator.h>
#include <voyx/sign/Oscillator.h>

template<typename T>
class Wobbulator : public Generator<T>
{

public:

  Wobbulator() :
    slope(0),
    intercept(0),
    lfo(),
    osc()
  {
  }

  Wobbulator(std::pair<T, T> frequencies, T period, const T samplerate) :
    slope((frequencies.first - frequencies.second) / T(2)),
    intercept((frequencies.first + frequencies.second) / T(2)),
    lfo(T(1) / period, samplerate),
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

  std::complex<T> operator()() override
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

private:

  T slope;
  T intercept;

  Oscillator<T> lfo;
  Oscillator<T> osc;

};
