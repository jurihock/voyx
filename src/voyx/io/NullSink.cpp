#include <voyx/io/NullSink.h>

#include <voyx/Source.h>

NullSink::NullSink(voyx_t samplerate, size_t framesize, size_t buffersize) :
  Sink(samplerate, framesize, buffersize)
{
}

bool NullSink::write(const size_t index, const voyx::vector<voyx_t> frame)
{
  return true;
}
