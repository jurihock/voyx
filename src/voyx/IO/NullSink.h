#pragma once

#include <voyx/Header.h>
#include <voyx/IO/Sink.h>

class NullSink : public Sink<float>
{

public:

  NullSink(size_t samplerate, size_t framesize, size_t buffersize);

  bool write(const size_t index, const std::vector<float>& frame) override;

};
