#include <voyx/io/SineSource.h>

#include <voyx/Source.h>

SineSource::SineSource(voyx_t frequency, voyx_t samplerate, size_t framesize, size_t buffersize) :
  SineSource(1, frequency, samplerate, framesize, buffersize)
{
}

SineSource::SineSource(voyx_t amplitude, voyx_t frequency, voyx_t samplerate, size_t framesize, size_t buffersize) :
  Source(samplerate, framesize, buffersize),
  amplitude(amplitude),
  frequency(frequency),
  osc(frequency, samplerate),
  frame(framesize)
{
}

bool SineSource::read(const size_t index, std::function<void(const voyx::vector<voyx_t> frame)> callback)
{
  for (size_t i = 0; i < frame.size(); ++i)
  {
    frame[i] = amplitude * osc.sin();
  }

  callback(frame);

  return true;
}
