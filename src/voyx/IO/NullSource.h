#pragma once

#include <voyx/IO/Source.h>

class NullSource : public Source<float>
{

public:

  NullSource(size_t samplerate, size_t framesize, size_t buffersize) :
    Source(samplerate, framesize, buffersize),
    frame(framesize)
  {
  }

  bool read(std::function<void(const std::vector<float>& frame)> callback) override
  {
    callback(frame);
    return true;
  }

private:

  const std::vector<float> frame;

};
