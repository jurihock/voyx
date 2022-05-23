#pragma once

#include <voyx/Header.h>

/**
 * Modulated Sliding DFT implementation according to [1] combined with [2].
 *
 * [1] Krzysztof Duda
 *     Accurate, Guaranteed Stable, Sliding Discrete Fourier Transform
 *     IEEE Signal Processing Magazine (2010)
 *     https://ieeexplore.ieee.org/document/5563098
 *
 * [2] Russell Bradford and Richard Dobson and John ffitch
 *     Sliding is Smoother than Jumping
 *     International Computer Music Conference (2005)
 *     https://quod.lib.umich.edu/cgi/p/pod/dod-idx/sliding-is-smoother-than-jumping.pdf?c=icmc;idno=bbp2372.2005.086;format=pdf
 **/
template <class T>
class MSDFT
{

public:

  MSDFT(const size_t size) :
    size(size)
  {
    roi.analysis = { 0, size };
    roi.synthesis = { 0, size };

    buffer.cursor = 0;
    buffer.input.resize(size);
    buffer.accoutput.resize(size);
    buffer.auxoutput.resize(size + 2);
    buffer.twiddles.resize(size);
    buffer.fiddles.resize(size);

    const T pi = T(-2) * std::acos(T(-1)) / size;

    for (size_t i = 0; i < size; ++i)
    {
      buffer.twiddles[i] = std::polar(T(1), pi * i);
    }
  }

  void sdft(const T sample, voyx::vector<std::complex<T>> dft)
  {
    voyxassert(dft.size() == size);

    const T scale = T(1) / size;
    const T delta = sample - std::exchange(buffer.input[buffer.cursor], sample);

    for (size_t i = roi.analysis.first, j = i + 1; i < roi.analysis.second; ++i, ++j)
    {
      const std::complex<T> oldfiddle = buffer.cursor ? buffer.fiddles[i] : 1;
      const std::complex<T> newfiddle = oldfiddle * buffer.twiddles[i];

      buffer.fiddles[i] = newfiddle;

      buffer.accoutput[i] += delta * oldfiddle;
      buffer.auxoutput[j] = buffer.accoutput[i] * std::conj(newfiddle);
    }

    buffer.auxoutput[0] = buffer.auxoutput[size];
    buffer.auxoutput[size + 1] = buffer.auxoutput[1];

    for (size_t i = roi.analysis.first, j = i + 1; i < roi.analysis.second; ++i, ++j)
    {
      dft[i] = window(buffer.auxoutput[j - 1],
                      buffer.auxoutput[j],
                      buffer.auxoutput[j + 1],
                      scale);
    }

    if (++buffer.cursor >= size)
    {
      buffer.cursor = 0;
    }
  }

  void sdft(const voyx::vector<T> samples, voyx::matrix<std::complex<T>> dfts)
  {
    voyxassert(samples.size() == dfts.size());

    for (size_t i = 0; i < samples.size(); ++i)
    {
      sdft(samples[i], dfts[i]);
    }
  }

  T isdft(const voyx::vector<std::complex<T>> dft)
  {
    voyxassert(dft.size() == size);

    T sample = T(0);

    for (size_t i = roi.synthesis.first; i < roi.synthesis.second; ++i)
    {
      sample += dft[i].real() * (i % 2 ? -1 : +1);
    }

    return sample;
  }

  void isdft(const voyx::matrix<std::complex<T>> dfts, voyx::vector<T> samples)
  {
    voyxassert(samples.size() == dfts.size());

    for (size_t i = 0; i < samples.size(); ++i)
    {
      samples[i] = isdft(dfts[i]);
    }
  }

private:

  const size_t size;

  struct
  {
    std::pair<size_t, size_t> analysis;
    std::pair<size_t, size_t> synthesis;
  }
  roi;

  struct
  {
    size_t cursor;
    std::vector<T> input;
    std::vector<std::complex<T>> accoutput;
    std::vector<std::complex<T>> auxoutput;
    std::vector<std::complex<T>> twiddles;
    std::vector<std::complex<T>> fiddles;
  }
  buffer;

  inline static std::complex<T> window(const std::complex<T>& left,
                                       const std::complex<T>& middle,
                                       const std::complex<T>& right,
                                       const T scale)
  {
    return T(0.25) * ((middle + middle) - (left + right)) * scale;
  }

};
