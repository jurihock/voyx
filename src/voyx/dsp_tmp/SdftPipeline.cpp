#include <voyx/DSP/SdftPipeline.h>

#include <voyx/Source.h>

SdftPipeline::SdftPipeline(const voyx_t samplerate, const size_t framesize, const size_t dftsize, std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink) :
  SyncPipeline<voyx_t>(source, sink),
  samplerate(samplerate),
  framesize(framesize),
  dftsize(dftsize),
  sdft(dftsize)
{
  data.dfts.resize(framesize * dftsize);
}

void SdftPipeline::operator()(const size_t index, const voyx::vector<voyx_t> input, voyx::vector<voyx_t> output)
{
  voyx::matrix<std::complex<voyx_t>> dfts(data.dfts, dftsize);

  sdft.sdft(input, dfts);
  (*this)(index, dfts);
  sdft.isdft(dfts, output);
}
