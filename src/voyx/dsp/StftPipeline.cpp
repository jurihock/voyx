#include <voyx/dsp/StftPipeline.h>

#include <voyx/Source.h>

StftPipeline::StftPipeline(const voyx_t samplerate, const size_t framesize, const size_t hopsize, std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink) :
  SyncPipeline<voyx_t>(source, sink),
  samplerate(samplerate),
  framesize(framesize),
  hopsize(hopsize),
  stft(framesize, hopsize)
{
  data.dfts.resize(stft.hops().size() * stft.size());
}

void StftPipeline::operator()(const size_t index, const voyx::vector<voyx_t> input, voyx::vector<voyx_t> output)
{
  voyx::matrix<std::complex<voyx_t>> dfts(data.dfts, stft.size());

  stft.stft(input, dfts);
  (*this)(index, stft.signal(), dfts);
  stft.istft(dfts, output);
}
