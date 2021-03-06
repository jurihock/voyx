#include <voyx/dsp/VoiceSynthPipeline.h>

#include <voyx/Source.h>

VoiceSynthPipeline::VoiceSynthPipeline(const voyx_t samplerate, const size_t framesize, const size_t hopsize,
                                       std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink,
                                       std::shared_ptr<MidiObserver> midi, std::shared_ptr<Plot> plot) :
  StftPipeline(samplerate, framesize, hopsize, source, sink),
  vocoder(samplerate, framesize, hopsize),
  midi(midi),
  plot(plot)
{
  if (midi != nullptr)
  {
  }

  if (plot != nullptr)
  {
    plot->xrange(samplerate / 2);
    plot->xlim(0, 5e3);
    plot->ylim(-120, 0);
  }
}

void VoiceSynthPipeline::operator()(const size_t index,
                                    const voyx::vector<voyx_t> signal,
                                    voyx::matrix<std::complex<voyx_t>> dfts)
{
  if (plot != nullptr)
  {
    FFT<voyx_t> fft(framesize * 2);

    std::vector<std::complex<voyx_t>> dft = fft.fft(signal);
    std::vector<voyx_t> abs(dft.size());

    for (size_t i = 0; i < dft.size(); ++i)
    {
      abs[i] = 20 * std::log10(std::abs(dft[i]));
    }

    plot->plot(abs);
  }

  vocoder.encode(dfts);

  const std::vector<voyx_t> factors = { 0.5, 1.25, 1.5, 2 };

  std::vector<std::complex<voyx_t>> buffer(factors.size() * dfts.stride());
  voyx::matrix<std::complex<voyx_t>> buffers(buffer, dfts.stride());

  for (auto dft : dfts)
  {
    for (size_t i = 0; i < factors.size(); ++i)
    {
      $$::interp(dft, buffers[i], factors[i]);
    }

    const auto mask = $$::argmax<$$::real>(buffers);

    for (size_t i = 0; i < dft.size(); ++i)
    {
      const size_t j = mask[i];

      dft[i] = buffers(j, i);

      dft[i].imag(dft[i].imag() * factors[j]);
    }
  }

  vocoder.decode(dfts);
}
