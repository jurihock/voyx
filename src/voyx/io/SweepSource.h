#pragma once

#include <voyx/Header.h>
#include <voyx/alg/Oscillator.h>
#include <voyx/io/Source.h>

class SweepSource : public Source<voyx_t>
{

public:

  SweepSource(std::pair<voyx_t, voyx_t> frequency, voyx_t duration, voyx_t samplerate, size_t framesize, size_t buffersize);
  SweepSource(voyx_t amplitude, std::pair<voyx_t, voyx_t> frequency, voyx_t duration, voyx_t samplerate, size_t framesize, size_t buffersize);

  bool read(const size_t index, std::function<void(const voyx::vector<voyx_t> frame)> callback) override;

private:

  const voyx_t amplitude;
  const std::pair<voyx_t, voyx_t> frequency;
  const voyx_t duration;

  Oscillator<voyx_t> lfo;
  Oscillator<voyx_t> sweep;

  std::vector<voyx_t> frame;

};
