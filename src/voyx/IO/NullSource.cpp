#include <voyx/IO/NullSource.h>

NullSource::NullSource(size_t samplerate, size_t framesize, size_t buffersize) :
  Source(samplerate, framesize, buffersize),
  frame(framesize)
{
}

bool NullSource::read(const size_t index, std::function<void(const std::vector<float>& frame)> callback)
{
  callback(frame);

  return true;
}
