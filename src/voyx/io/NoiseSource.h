#pragma once

#include <voyx/Header.h>
#include <voyx/io/Source.h>

class NoiseSource : public Source<voyx_t>
{

public:

  NoiseSource(voyx_t samplerate, size_t framesize, size_t buffersize);
  NoiseSource(voyx_t amplitude, voyx_t samplerate, size_t framesize, size_t buffersize);

  bool read(const size_t index, std::function<void(const voyx::vector<voyx_t> frame)> callback) override;

private:

  const voyx_t amplitude;

  std::random_device seed;
  std::mt19937 generator;
  std::uniform_real_distribution<voyx_t> distribution;
  std::vector<voyx_t> frame;

};
