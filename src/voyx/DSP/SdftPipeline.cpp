#include <voyx/DSP/SdftPipeline.h>

#include <voyx/Source.h>

SdftPipeline::SdftPipeline(const voyx_t samplerate, const size_t framesize, const size_t dftsize, std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink) :
  Pipeline(source, sink),
  samplerate(samplerate),
  framesize(framesize),
  dftsize(dftsize),
  sdft(dftsize)
{
  data.dfts.resize(framesize * dftsize);
  data.views.dfts.resize(framesize);

  for (size_t i = 0; i < framesize; ++i)
  {
    data.views.dfts[i] = std::span<std::complex<voyx_t>>(
      data.dfts.data() + i * dftsize,
      dftsize);
  }
}

void SdftPipeline::operator()(const size_t index, const std::vector<voyx_t>& input, std::vector<voyx_t>& output)
{
  sdft.sdft(input, data.views.dfts);
  (*this)(index, data.views.dfts);
  sdft.isdft(data.views.dfts, output);
}
