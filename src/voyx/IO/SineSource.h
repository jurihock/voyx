#pragma once

#include <voyx/Header.h>
#include <voyx/IO/Source.h>

class SineSource : public Source<voyx_t>
{

public:

  SineSource(voyx_t frequency, voyx_t samplerate, size_t framesize, size_t buffersize);
  SineSource(voyx_t amplitude, voyx_t frequency, voyx_t samplerate, size_t framesize, size_t buffersize);
  ~SineSource();

  bool read(const size_t index, std::function<void(const voyx::vector<voyx_t> frame)> callback) override;

private:

  const voyx_t PI2 = voyx_t(2) * std::acos(voyx_t(-1));

  const voyx_t amplitude;
  const voyx_t frequency;
  voyx_t phase;

  voyx::vector<voyx_t> frame;

};
