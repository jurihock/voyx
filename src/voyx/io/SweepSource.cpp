#include <voyx/io/SweepSource.h>

#include <voyx/Source.h>

SweepSource::SweepSource(std::pair<voyx_t, voyx_t> frequencies, voyx_t duration, voyx_t samplerate, size_t framesize, size_t buffersize) :
  SweepSource(1, frequencies, duration, samplerate, framesize, buffersize)
{
}

SweepSource::SweepSource(voyx_t amplitude, std::pair<voyx_t, voyx_t> frequencies, voyx_t duration, voyx_t samplerate, size_t framesize, size_t buffersize) :
  Source(samplerate, framesize, buffersize),
  amplitude(amplitude),
  frequencies(frequencies),
  duration(duration),
  lfo(1 / duration, samplerate),
  sine(frequencies.first, samplerate),
  frame(framesize)
{
}

bool SweepSource::read(const size_t index, std::function<void(const voyx::vector<voyx_t> frame)> callback)
{
  const voyx_t slope = (frequencies.second - frequencies.first) / 2;
  const voyx_t intercept = (frequencies.second + frequencies.first) / 2;

  for (size_t i = 0; i < frame.size(); ++i)
  {
    const voyx_t frequency = lfo() * slope + intercept;

    frame[i] = amplitude * sine(frequency);
  }

  callback(frame);

  return true;
}
