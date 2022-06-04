#include <voyx/dsp/RobotPipeline.h>

#include <voyx/Source.h>

RobotPipeline::RobotPipeline(const voyx_t samplerate, const size_t framesize, const size_t dftsize,
                             std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink,
                             std::shared_ptr<MidiObserver> midi, std::shared_ptr<Plot> plot) :
  SdftPipeline(samplerate, framesize, dftsize, source, sink),
  midi(midi),
  plot(plot),
  osc(dftsize),
  f0(0)
{
}

void RobotPipeline::operator()(const size_t index,
                               voyx::matrix<std::complex<voyx_t>> dfts)
{
  voyx_t f0 = 0;

  if (midi != nullptr)
  {
    const auto state = midi->state();

    for (voyx_t i = 0; i < state.size(); ++i)
    {
      if (state[i])
      {
        f0 = $$::midi::freq(i, midi->concertpitch());
        break;
      }
    }
  }

  if (f0)
  {
    if (f0 != this->f0)
    {
      this->f0 = f0;

      for (size_t i = 0; i < dftsize; ++i)
      {
        osc[i] = { i * f0, samplerate };
      }
    }

    for (size_t i = 0; i < dfts.size(); ++i)
    {
      auto dft = dfts[i];

      for (size_t j = 0; j < dft.size(); ++j)
      {
        dft[j] = std::abs(dft[j]) * osc[j];
      }
    }
  }
}
