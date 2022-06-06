#pragma once

#include <voyx/Header.h>
#include <voyx/io/Source.h>
#include <voyx/sign/Wobbulator.h>

class SweepSource : public Source<voyx_t>
{

public:

  SweepSource(std::pair<voyx_t, voyx_t> frequencies, voyx_t period, voyx_t samplerate, size_t framesize, size_t buffersize);
  SweepSource(voyx_t amplitude, std::pair<voyx_t, voyx_t> frequencies, voyx_t period, voyx_t samplerate, size_t framesize, size_t buffersize);

  bool read(const size_t index, std::function<void(const voyx::vector<voyx_t> frame)> callback) override;

private:

  const voyx_t amplitude;
  const std::pair<voyx_t, voyx_t> frequencies;
  const voyx_t period;

  Wobbulator<voyx_t> osc;

  std::vector<voyx_t> frame;

};
