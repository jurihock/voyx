#pragma once

#include <voyx/IO/Source.h>

class SineSource : public Source<float>
{

public:

  SineSource(float frequency, size_t samplerate, size_t framesize, size_t buffersize);
  SineSource(float amplitude, float frequency, size_t samplerate, size_t framesize, size_t buffersize);

  bool read(const size_t index, std::function<void(const std::vector<float>& frame)> callback) override;

private:

  const float PI2 = 2.0f * std::acos(-1.0f);

  const float amplitude;
  const float frequency;
  float phase;

  std::vector<float> frame;

};
