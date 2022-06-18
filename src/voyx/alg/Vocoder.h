#pragma once

#include <voyx/Header.h>

template<typename T>
class Vocoder
{

public:

  Vocoder(const voyx_t samplerate, const size_t framesize, const size_t hopsize) :
    framesize(framesize),
    hopsize(hopsize),
    dftsize(framesize / 2),
    freqinc(samplerate / framesize),
    phaseinc(pi * hopsize / framesize)
  {
    analysis.buffer.resize(dftsize + 1);
    synthesis.buffer.resize(dftsize + 1);
  }

  void encode(voyx::matrix<std::complex<T>> dfts)
  {
    for (auto dft : dfts)
    {
      encode(dft);
    }
  }

  void decode(voyx::matrix<std::complex<T>> dfts)
  {
    for (auto dft : dfts)
    {
      decode(dft);
    }
  }

  void encode(voyx::vector<std::complex<T>> dft)
  {
    T frequency,
      phase,
      delta,
      j;

    for (size_t i = 0; i < dft.size(); ++i)
    {
      phase = arg(dft[i]); // instead of std::arg(dft[i])

      delta = phase - std::exchange(analysis.buffer[i], phase);

      j = wrap(delta - i * phaseinc) / phaseinc;

      frequency = (i + j) * freqinc;

      dft[i] = std::complex<T>(std::abs(dft[i]), frequency);
    }
  }

  void decode(voyx::vector<std::complex<T>> dft)
  {
    T frequency,
      phase,
      delta,
      j;

    for (size_t i = 0; i < dft.size(); ++i)
    {
      frequency = dft[i].imag();

      j = (frequency - i * freqinc) / freqinc;

      delta = (i + j) * phaseinc;

      phase = synthesis.buffer[i] += delta;

      dft[i] = std::polar<T>(dft[i].real(), phase);
    }
  }

private:

  const T pi = T(2) * std::acos(T(-1));

  const size_t framesize;
  const size_t hopsize;
  const size_t dftsize;

  const T freqinc;
  const T phaseinc;

  struct
  {
    std::vector<T> buffer;
  }
  analysis;

  struct
  {
    std::vector<T> buffer;
  }
  synthesis;

  /**
   * Converts the specified arbitrary phase value
   * to be within the interval from -pi to pi.
   **/
  inline static T wrap(const T phase)
  {
    const T PI = T(2) * T(M_PI);

    return phase - PI * std::floor(phase / PI + T(0.5));
  }

  /**
   * Approximates the phase angle of the complex number z.
   **/
  inline static T arg(const std::complex<T>& z)
  {
    return atan2(z.imag(), z.real());
  }

  /**
   * Approximates the arcus tangent of y/x according to [1] and [2].
   *
   * [1] Sreeraman Rajan and Sichun Wang and Robert Inkol and Alain Joyal
   *     Efficient approximations for the arctangent function
   *     IEEE Signal Processing Magazine (2006)
   *     https://ieeexplore.ieee.org/document/1628884
   *
   * [2] Dmytro Mishkin
   *     https://github.com/ducha-aiki/fast_atan2
   **/
  inline static T atan2(const T y, const T x)
  {
    const T PI = T(M_PI);
    const T PI2 = T(M_PI_2);
    const T PI4 = T(M_PI_4);

    const T A = PI4 + T(0.273);
    const T B = T(0.273);

    const T absy = std::abs(y);
    const T absx = std::abs(x);

    const int octant = ((absx <= absy) << 2) + ((y < 0) << 1) + ((x < 0) << 0);

    if (x == 0 && y == 0)
    {
      return T(0);
    }

    if (x == 0)
    {
      return (y > 0) ? +PI2 : -PI2;
    }

    if (y == 0)
    {
      return (x > 0) ? 0 : PI;
    }

    switch (octant)
    {
      case 0: // i
      {
        const T q = absy / absx;

        return +(A - B * q) * q;
      }
      case 4: // ii
      {
        const T q = absx / absy;

        return +PI2 - (A - B * q) * q;
      }
      case 5: // iii
      {
        const T q = absx / absy;

        return +PI2 + (A - B * q) * q;
      }
      case 1: // iv
      {
        const T q = absy / absx;

        return +PI - (A - B * q) * q;
      }
      case 3: // v
      {
        const T q = absy / absx;

        return -PI + (A - B * q) * q;
      }
      case 7: // vi
      {
        const T q = absx / absy;

        return -PI2 - (A - B * q) * q;
      }
      case 6: // vii
      {
        const T q = absx / absy;

        return -PI2 + (A - B * q) * q;
      }
      case 2: // viii
      {
        const T q = absy / absx;

        return -(A - B * q) * q;
      }
      default:
      {
        return T(0);
      }
    }
  }
};
