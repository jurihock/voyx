#pragma once

#include <voyx/Header.h>
#include <voyx/ALG/SDFT.h>
#include <voyx/DSP/SyncPipeline.h>

class SdftPipeline : public SyncPipeline<voyx_t>
{

public:

  SdftPipeline(const voyx_t samplerate, const size_t framesize, const size_t dftsize, std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink);

protected:

  const voyx_t samplerate;
  const size_t framesize;
  const size_t dftsize;

  void operator()(const size_t index, const voyx::vector<voyx_t> input, voyx::vector<voyx_t> output) override;

  virtual void operator()(const size_t index, voyx::matrix<std::complex<voyx_t>> dfts) = 0;

private:

  SDFT<voyx_t> sdft;

  struct
  {
    std::vector<std::complex<voyx_t>> dfts;
  }
  data;

};
