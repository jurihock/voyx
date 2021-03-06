#pragma once

#include <voyx/Header.h>
#include <voyx/io/Sink.h>

class NullSink : public Sink<voyx_t>
{

public:

  NullSink(voyx_t samplerate, size_t framesize, size_t buffersize);

  bool write(const size_t index, const voyx::vector<voyx_t> frame) override;

};
