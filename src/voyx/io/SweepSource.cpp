#include <voyx/io/SweepSource.h>

#include <voyx/Source.h>

SweepSource::SweepSource(std::pair<voyx_t, voyx_t> frequency, voyx_t duration, voyx_t samplerate, size_t framesize, size_t buffersize) :
  SweepSource(1, frequency, duration, samplerate, framesize, buffersize)
{
}

SweepSource::SweepSource(voyx_t amplitude, std::pair<voyx_t, voyx_t> frequency, voyx_t duration, voyx_t samplerate, size_t framesize, size_t buffersize) :
  Source(samplerate, framesize, buffersize),
  amplitude(amplitude),
  frequency(frequency),
  duration(duration),
  lfo(1 / duration, samplerate),
  sweep(frequency.first, samplerate),
  frame(framesize)
{
}

bool SweepSource::read(const size_t index, std::function<void(const voyx::vector<voyx_t> frame)> callback)
{
  for (size_t i = 0; i < frame.size(); ++i)
  {
    const voyx_t f = (lfo() * 0.5 + 0.5) * (frequency.second - frequency.first) + (frequency.first);

    frame[i] = amplitude * sweep(f);
  }

  callback(frame);

  return true;
}
