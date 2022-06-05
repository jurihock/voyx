#pragma once

#include <voyx/Header.h>

/**
 * Phase estimation from DFT magnitude according to [1].
 *
 * [1] Gerald Beauregard and Mithila Harish and Lonce Wyse
 *     Single Pass Spectrogram Inversion
 *     IEEE International Conference on Digital Signal Processing (2015)
 *     https://ieeexplore.ieee.org/document/7251907
 **/
template<typename T>
class SPSI
{

public:

  SPSI(const size_t framesize, const size_t hopsize) :
    framesize(framesize),
    hopsize(hopsize),
    dftsize(framesize / 2),
    abs(dftsize + 1),
    arg(dftsize + 1),
    cursor(0)
  {
  }

  void operator()(voyx::vector<std::complex<T>> dft)
  {
    const T pi = T(1) * std::acos(T(-1));
    const T inc = T(2) * pi * hopsize / framesize;

    for (size_t i = 0; i < dft.size(); ++i)
    {
      abs[i] = std::abs(dft[i]);
    }

    for (size_t i = 1; i < dft.size() - 1; ++i)
    {
      if (abs[i] < abs[i - 1] || abs[i] < abs[i + 1])
      {
        continue;
      }

      const T j = interp(abs[i - 1], abs[i], abs[i + 1]);

      arg[i] += (i + j) * inc;

      size_t bin;

      if (j > 0)
      {
        bin = i + 1;

        arg[bin] = arg[i] + pi;

        bin = i - 1;

        while((bin > 1) && (abs[bin] < abs[bin + 1]))
        {
          arg[bin--] = arg[i] + pi;
        }

        bin = i + 2;

        while((bin < dft.size()) && (abs[bin] < abs[bin - 1]))
        {
          arg[bin++] = arg[i];
        }
      }

      if (j < 0)
      {
        bin = i - 1;

        arg[bin] = arg[i] + pi;

        bin = i + 1;

        while((bin < dft.size()) && (abs[bin] < abs[bin - 1]))
        {
          arg[bin++] = arg[i] + pi;
        }

        bin = i - 2;

        while((bin > 1) && (abs[bin] < abs[bin + 1]))
        {
          arg[bin--] = arg[i];
        }
      }
    }

    for (size_t i = 0; i < dft.size(); ++i)
    {
      dft[i] = std::polar(abs[i], arg[i]);
    }

    // TODO fix phase drift

    // if ((cursor += hopsize) >= framesize) // sync with sdft
    // {
    //   cursor = 0;

    //   std::fill(arg.begin(), arg.end(), 0);
    // }
  }

  void operator()(voyx::matrix<std::complex<T>> dfts)
  {
    for (auto dft : dfts)
    {
      (*this)(dft);
    }
  }

private:

  const size_t framesize;
  const size_t hopsize;
  const size_t dftsize;

  std::vector<T> abs;
  std::vector<T> arg;

  size_t cursor;

  inline static T interp(const T left, const T middle, const T right)
  {
    const T a = (left - right) * T(0.5);
    const T b = (left + right) - T(2) * middle;

    return (b != 0) ? a / b : T(0);
  }

};
