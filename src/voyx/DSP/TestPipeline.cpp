#include <voyx/DSP/TestPipeline.h>

#include <voyx/Source.h>

TestPipeline::TestPipeline(const size_t samplerate, const size_t framesize, const size_t hopsize, std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink, std::shared_ptr<Plot> plot) :
  StftPipeline(samplerate, framesize, hopsize, source, sink),
  fft(framesize * 2),
  plot(plot)
{
  if (plot != nullptr)
  {
    plot->xrange(samplerate / 2);
    plot->xlim(0, 5e3);
    plot->ylim(-120, 0);
  }
}

void TestPipeline::operator()(const size_t index, const std::vector<voyx_t>& signal, const std::vector<std::span<std::complex<voyx_t>>>& dfts)
{
  if (plot != nullptr)
  {
    std::vector<std::complex<voyx_t>> dft = fft.fft(signal);
    std::vector<voyx_t> abs(dft.size());

    for (size_t i = 0; i < dft.size(); ++i)
    {
      abs[i] = 20 * std::log10(std::abs(dft[i]));
    }

    plot->plot(abs);
  }
}
