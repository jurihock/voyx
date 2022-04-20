#include <voyx/IO/SineSource.h>

#include <voyx/Source.h>

SineSource::SineSource(voyx_t frequency, voyx_t samplerate, size_t framesize, size_t buffersize) :
  SineSource(1.0f, frequency, samplerate, framesize, buffersize)
{
}

SineSource::SineSource(voyx_t amplitude, voyx_t frequency, voyx_t samplerate, size_t framesize, size_t buffersize) :
  Source(samplerate, framesize, buffersize),
  amplitude(amplitude),
  frequency(frequency),
  phase(0),
  frame(framesize)
{
}

bool SineSource::read(const size_t index, std::function<void(const std::vector<voyx_t>& frame)> callback)
{
  const voyx_t phaseinc = frequency / samplerate();

  for (size_t i = 0; i < frame.size(); ++i)
  {
    phase += phaseinc;

    if (phase >= 1)
    {
      phase -= 1;
    }

    frame[i] = amplitude * std::sin(PI2 * phase);
  }

  callback(frame);

  return true;
}
