#pragma once

#include <voyx/Header.h>

template<typename T>
class SpectralPitchDetector
{

public:

  SpectralPitchDetector(const std::pair<voyx_t, voyx_t> roi, const voyx_t samplerate) :
    roi(roi),
    samplerate(samplerate)
  {
  }

  voyx_t operator()(const voyx::vector<T> spectrum) const
  {
    const size_t framesize = spectrum.size() * 2;

    const size_t nmin = size_t(0);
    const size_t nmax = spectrum.size();

    const size_t fmin = static_cast<size_t>(roi.first * framesize / samplerate);
    const size_t fmax = static_cast<size_t>(roi.second * framesize / samplerate);

    const size_t imin = std::clamp(std::min(fmin, fmax), nmin, nmax);
    const size_t imax = std::clamp(std::max(fmin, fmax), nmin, nmax);

    voyxassert(imin < imax);

    T value = std::numeric_limits<T>::lowest();
    ptrdiff_t index = -1;

    for (size_t i = imin; i < imax; ++i)
    {
      if (spectrum[i] > value)
      {
        value = spectrum[i];
        index = i;
      }
    }

    return index * samplerate / framesize;
  }

private:

  const std::pair<voyx_t, voyx_t> roi;
  const voyx_t samplerate;

};
