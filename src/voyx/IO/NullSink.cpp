#include <voyx/IO/NullSink.h>

#include <voyx/Source.h>

NullSink::NullSink(size_t samplerate, size_t framesize, size_t buffersize) :
  Sink(samplerate, framesize, buffersize)
{
}

bool NullSink::write(const size_t index, const std::vector<float>& frame)
{
  return true;
}
