#include <voyx/io/NoiseSource.h>

#include <voyx/Source.h>

NoiseSource::NoiseSource(voyx_t samplerate, size_t framesize, size_t buffersize) :
  NoiseSource(1, samplerate, framesize, buffersize)
{
}

NoiseSource::NoiseSource(voyx_t amplitude, voyx_t samplerate, size_t framesize, size_t buffersize) :
  Source(samplerate, framesize, buffersize),
  amplitude(amplitude),
  noise(),
  frame(framesize)
{
}

bool NoiseSource::read(const size_t index, std::function<void(const voyx::vector<voyx_t> frame)> callback)
{
  for (size_t i = 0; i < frame.size(); ++i)
  {
    frame[i] = amplitude * noise++;
  }

  callback(frame);

  return true;
}
