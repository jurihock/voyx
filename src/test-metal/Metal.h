#pragma once

namespace voyx
{
  class complex
  {

  public:

    complex() :
      value(float2(0, 0))
    {
    }

    complex(const float real) :
      value(float2(real, 0))
    {
    }

    complex(const float real, const float imag) :
      value(float2(real, imag))
    {
    }

    complex(const float2 value) :
      value(value)
    {
    }

    float real() const { return value[0]; }
    float imag() const { return value[1]; }

    friend complex operator+(const complex left, const complex right)
    {
      const float real = left.real() + right.real();
      const float imag = left.imag() + right.imag();

      return complex(real, imag);
    }

    friend complex operator-(const complex left, const complex right)
    {
      const float real = left.real() - right.real();
      const float imag = left.imag() - right.imag();

      return complex(real, imag);
    }

    friend complex operator*(const complex left, const complex right)
    {
      const float real = left.real() * right.real() - left.imag() * right.imag();
      const float imag = left.real() * right.imag() + left.imag() * right.real();

      return complex(real, imag);
    }

    friend complex operator*(const complex left, const float right)
    {
      const float real = left.real() * right;
      const float imag = left.imag() * right;

      return complex(real, imag);
    }

    friend complex operator*(const float left, const complex right)
    {
      const float real = left * right.real();
      const float imag = left * right.imag();

      return complex(real, imag);
    }

  private:

    float2 value;

  };
}
