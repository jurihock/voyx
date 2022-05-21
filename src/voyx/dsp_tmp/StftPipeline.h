#pragma once

#include <voyx/Header.h>
#include <voyx/ALG/STFT.h>
#include <voyx/DSP/SyncPipeline.h>

class StftPipeline : public SyncPipeline<voyx_t>
{

public:

  StftPipeline(const voyx_t samplerate, const size_t framesize, const size_t hopsize, std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink);

protected:

  const voyx_t samplerate;
  const size_t framesize;
  const size_t hopsize;

  void operator()(const size_t index, const voyx::vector<voyx_t> input, voyx::vector<voyx_t> output) override;

  virtual void operator()(const size_t index, const voyx::vector<voyx_t> signal, voyx::matrix<std::complex<voyx_t>> dfts) = 0;

private:

  STFT<voyx_t> stft;

  struct
  {
    std::vector<std::complex<voyx_t>> dfts;
  }
  data;

};
