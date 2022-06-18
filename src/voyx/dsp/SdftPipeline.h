#pragma once

#include <voyx/Header.h>
#include <voyx/alg/SDFT.h>
#include <voyx/dsp/SyncPipeline.h>

template<typename T = voyx_t>
class SdftPipeline : public SyncPipeline<voyx_t>
{

public:

  SdftPipeline(const voyx_t samplerate, const size_t framesize, const size_t dftsize, std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink) :
    SyncPipeline<voyx_t>(source, sink),
    samplerate(samplerate),
    framesize(framesize),
    dftsize(dftsize),
    sdft(dftsize)
  {
    data.dfts.resize(framesize * dftsize);
  }

protected:

  const voyx_t samplerate;
  const size_t framesize;
  const size_t dftsize;

  void operator()(const size_t index, const voyx::vector<voyx_t> input, voyx::vector<voyx_t> output) override
  {
    voyx::matrix<std::complex<T>> dfts(data.dfts, dftsize);

    sdft.sdft(input, dfts);
    (*this)(index, dfts);
    sdft.isdft(dfts, output);
  }

  virtual void operator()(const size_t index, voyx::matrix<std::complex<T>> dfts) = 0;

private:

  SDFT<voyx_t, T> sdft;

  struct
  {
    std::vector<std::complex<T>> dfts;
  }
  data;

};
