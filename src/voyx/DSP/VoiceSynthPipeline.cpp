#include <voyx/DSP/VoiceSynthPipeline.h>

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

void VoiceSynthPipeline::operator()(const size_t index, const std::vector<voyx_t>& signal,
                                    const std::vector<std::span<std::complex<voyx_t>>>& dfts)
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

  const voyx_t factor = 2;

  for (auto dft : dfts)
  {
    $$::interp(dft.size(), dft.data(), dft.data(), factor);

    for (size_t i = 0; i < dft.size(); ++i)
    {
      dft[i].imag(dft[i].imag() * factor);
    }
  }

  vocoder.decode(dfts);
}
