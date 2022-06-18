#pragma once

#include <voyx/Header.h>
#include <voyx/alg/STFT.h>
#include <voyx/dsp/SyncPipeline.h>

template<typename T = voyx_t>
class StftPipeline : public SyncPipeline<voyx_t>
{

public:

  StftPipeline(const voyx_t samplerate, const size_t framesize, const size_t hopsize, std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink) :
    SyncPipeline<voyx_t>(source, sink),
    samplerate(samplerate),
    framesize(framesize),
    hopsize(hopsize),
    stft(framesize, hopsize)
  {
    data.dfts.resize(stft.hops().size() * stft.size());
  }

protected:

  const voyx_t samplerate;
  const size_t framesize;
  const size_t hopsize;

  void operator()(const size_t index, const voyx::vector<voyx_t> input, voyx::vector<voyx_t> output) override
  {
    voyx::matrix<std::complex<T>> dfts(data.dfts, stft.size());

    stft.stft(input, dfts);
    (*this)(index, stft.signal(), dfts);
    stft.istft(dfts, output);
  }

  virtual void operator()(const size_t index, const voyx::vector<voyx_t> signal, voyx::matrix<std::complex<T>> dfts) = 0;

private:

  STFT<voyx_t, T> stft;

  struct
  {
    std::vector<std::complex<T>> dfts;
  }
  data;

};
