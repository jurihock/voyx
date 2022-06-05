#include <voyx/dsp/RobotPipeline.h>

#include <voyx/Source.h>

RobotPipeline::RobotPipeline(const voyx_t samplerate, const size_t framesize, const size_t dftsize,
                             std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink,
                             std::shared_ptr<MidiObserver> midi, std::shared_ptr<Plot> plot) :
  SdftPipeline(samplerate, framesize, dftsize, source, sink),
  midi(midi),
  plot(plot)
{
}

void RobotPipeline::operator()(const size_t index,
                               voyx::matrix<std::complex<voyx_t>> dfts)
{
  std::set<voyx_t> frequencies;
  bool sustain = false;

  if (midi != nullptr)
  {
    const auto values = midi->frequencies();
    frequencies.insert(values.begin(), values.end());

    sustain = midi->sustain();
  }

  if (sustain)
  {
    frequencies.merge(this->frequencies);
  }

  this->frequencies = frequencies;

  for (const voyx_t frequency : frequencies)
  {
    if (osc.count(frequency))
    {
      continue;
    }

    osc[frequency].resize(dftsize);

    for (size_t i = 0; i < dftsize; ++i)
    {
      osc[frequency][i] = { i * frequency, samplerate };
    }
  }

  std::vector<voyx_t> abs(dftsize);

  for (size_t i = 0; i < dfts.size(); ++i)
  {
    auto dft = dfts[i];

    for (size_t j = 0; j < dft.size(); ++j)
    {
      abs[j] = std::abs(dft[j]);
      dft[j] = 0;
    }

    for (const voyx_t frequency : frequencies)
    {
      for (size_t j = 0; j < dft.size(); ++j)
      {
        dft[j] += osc[frequency][j];
      }
    }

    const voyx_t weight = frequencies.empty()
      ? voyx_t(0) : voyx_t(1) / frequencies.size();

    for (size_t j = 0; j < dft.size(); ++j)
    {
      dft[j] *= abs[j] * weight;
    }
  }
}
