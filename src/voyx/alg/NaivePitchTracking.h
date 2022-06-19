#pragma once

#include <voyx/Header.h>
#include <voyx/etc/Convert.h>

class NaivePitchTracking
{

public:

  NaivePitchTracking(const voyx_t concertpitch) :
    concertpitch(concertpitch)
  {
  }

  voyx_t operator()(const voyx_t f0) const
  {
    const voyx_t key = std::round($$::midi::key(f0, concertpitch));
    const voyx_t freq = $$::midi::freq(key, concertpitch);

    return freq;
  }

private:

  const voyx_t concertpitch;

};
