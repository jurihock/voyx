#pragma once

#include <voyx/Header.h>
#include <voyx/io/Source.h>
#include <voyx/sign/Oscillator.h>

class SineSource : public Source<voyx_t>
{

public:

  SineSource(voyx_t frequency, voyx_t samplerate, size_t framesize, size_t buffersize);
  SineSource(voyx_t amplitude, voyx_t frequency, voyx_t samplerate, size_t framesize, size_t buffersize);

  bool read(const size_t index, std::function<void(const voyx::vector<voyx_t> frame)> callback) override;

private:

  const voyx_t amplitude;
  const voyx_t frequency;

  Oscillator<voyx_t> osc;

  std::vector<voyx_t> frame;

};
