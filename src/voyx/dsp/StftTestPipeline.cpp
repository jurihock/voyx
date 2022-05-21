#include <voyx/dsp/StftTestPipeline.h>

#include <voyx/Source.h>

StftTestPipeline::StftTestPipeline(const voyx_t samplerate, const size_t framesize, const size_t hopsize,
                                   std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink,
                                   std::shared_ptr<MidiObserver> midi, std::shared_ptr<Plot> plot) :
  StftPipeline(samplerate, framesize, hopsize, source, sink),
  midi(midi),
  plot(plot)
{
  if (plot != nullptr)
  {
    plot->xrange(samplerate / 2);
    plot->xlim(0, 2e3);
    plot->ylim(-120, 0);
  }
}

void StftTestPipeline::operator()(const size_t index,
                                  const voyx::vector<voyx_t> signal,
                                  voyx::matrix<std::complex<voyx_t>> dfts)
{
  if (plot != nullptr)
  {
    const auto dft = dfts.front();

    std::vector<voyx_t> abs(dft.size());

    for (size_t i = 0; i < dft.size(); ++i)
    {
      abs[i] = 20 * std::log10(std::abs(dft[i]));
    }

    plot->plot(abs);
  }
}
