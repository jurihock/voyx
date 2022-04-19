#pragma once

#include <voyx/Header.h>
#include <voyx/IO/Sink.h>

class NullSink : public Sink<voyx_t>
{

public:

  NullSink(size_t samplerate, size_t framesize, size_t buffersize);

  bool write(const size_t index, const std::vector<voyx_t>& frame) override;

};
