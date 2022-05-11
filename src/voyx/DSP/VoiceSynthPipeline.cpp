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

  const std::vector<voyx_t> factors = { 0.5, 1.25, 1.5, 2 };

  std::vector<std::complex<voyx_t>> buffer(factors.size() * dfts.front().size());
  std::vector<std::span<std::complex<voyx_t>>> buffers(factors.size());
  std::vector<size_t> mask(dfts.front().size());

  for (size_t i = 0; i < factors.size(); ++i)
  {
    buffers[i] = std::span<std::complex<voyx_t>>(
      buffer.data() + i * dfts.front().size(),
      dfts.front().size());
  }

  for (auto dft : dfts)
  {
    for (size_t i = 0; i < factors.size(); ++i)
    {
      // FIXME $$::interp(dft, buffers[i], factors[i]);
      $$::interp(dft.size(), dft.data(), buffers[i].data(), factors[i]);
    }

    for (size_t i = 0; i < dfts.front().size(); ++i)
    {
      size_t index = 0;
      voyx_t value = buffers[0][i].real();

      for (size_t j = 1; j < factors.size(); ++j)
      {
        if (buffers[j][i].real() > value)
        {
          index = j;
          value = buffers[j][i].real();
        }
      }

      mask[i] = index;
    }

    for (size_t i = 0; i < dft.size(); ++i)
    {
      const size_t j = mask[i];

      dft[i].real(buffers[j][i].real());
      dft[i].imag(buffers[j][i].imag() * factors[j]);
    }
  }

  vocoder.decode(dfts);
}
