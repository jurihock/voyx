#pragma once

#include <voyx/Header.h>
#include <voyx/ETC/SDFT.h>
#include <voyx/DSP/Pipeline.h>

class SdftPipeline : public Pipeline<voyx_t>
{

public:

  SdftPipeline(const voyx_t samplerate, const size_t framesize, const size_t dftsize, std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink);

  void operator()(const size_t index, const voyx::vector<voyx_t> input, voyx::vector<voyx_t> output) override;

protected:

  const voyx_t samplerate;
  const size_t framesize;
  const size_t dftsize;

  virtual void operator()(const size_t index, voyx::matrix<std::complex<voyx_t>> dfts) = 0;

private:

  SDFT<voyx_t> sdft;

  struct
  {
    std::vector<std::complex<voyx_t>> dfts;
  }
  data;

};
