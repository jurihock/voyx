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
  omega(std::polar<voyx_t>(1, pi * frequency / samplerate)),
  phasor(1),
  frame(framesize)
{
}

bool SineSource::read(const size_t index, std::function<void(const voyx::vector<voyx_t> frame)> callback)
{
  for (size_t i = 0; i < frame.size(); ++i)
  {
    frame[i] = amplitude * phasor.imag();

    phasor *= omega;
  }

  callback(frame);

  return true;
}
