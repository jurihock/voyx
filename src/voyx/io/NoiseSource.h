#pragma once

#include <voyx/Header.h>
#include <voyx/io/Source.h>
#include <voyx/sign/Noise.h>

class NoiseSource : public Source<voyx_t>
{

public:

  NoiseSource(voyx_t samplerate, size_t framesize, size_t buffersize);
  NoiseSource(voyx_t amplitude, voyx_t samplerate, size_t framesize, size_t buffersize);

  bool read(const size_t index, std::function<void(const voyx::vector<voyx_t> frame)> callback) override;

private:

  const voyx_t amplitude;

  Noise<voyx_t> noise;

  std::vector<voyx_t> frame;

};
