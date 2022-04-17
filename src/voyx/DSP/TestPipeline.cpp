#include <voyx/DSP/TestPipeline.h>

#include <voyx/Source.h>

TestPipeline::TestPipeline(const size_t samplerate, const size_t framesize, const size_t hopsize, std::shared_ptr<Plot> plot, std::shared_ptr<Source<float>> source, std::shared_ptr<Sink<float>> sink) :
  StftPipeline(samplerate, framesize, hopsize, source, sink),
  plot(plot)
{
  if (plot != nullptr)
  {
    plot->xrange(samplerate / 2);
    plot->xlim(0, 5e3);
    plot->ylim(-120, 0);
  }
}

void TestPipeline::operator()(std::vector<std::vector<std::complex<float>>>& dfts)
{
  const auto& dft = dfts.front();

  std::vector<float> abs(dft.size());

  for (size_t i = 0; i < dft.size(); ++i)
  {
    abs[i] = 20 * std::log10(std::abs(dft[i]));
  }

  if (plot != nullptr)
  {
    plot->plot(abs);
  }
}
