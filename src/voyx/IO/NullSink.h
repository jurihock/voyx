#pragma once

#include <voyx/IO/Sink.h>

class NullSink : public Sink<float>
{

public:

  NullSink(size_t samplerate, size_t framesize, size_t buffersize) :
    Sink(samplerate, framesize, buffersize)
  {
  }

  bool write(const std::vector<float>& frame) override
  {
    return true;
  }

};
