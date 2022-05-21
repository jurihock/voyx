#include <voyx/io/NullSource.h>

#include <voyx/Source.h>

NullSource::NullSource(voyx_t samplerate, size_t framesize, size_t buffersize) :
  Source(samplerate, framesize, buffersize),
  frame(framesize)
{
}

bool NullSource::read(const size_t index, std::function<void(const voyx::vector<voyx_t> frame)> callback)
{
  callback(frame);

  return true;
}
