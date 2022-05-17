#pragma once

#include <voyx/Header.h>
#include <voyx/ETC/FFT.h>
#include <voyx/ETC/Window.h>
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

  const FFT<voyx_t> fft;

  std::vector<size_t> hops;

  struct
  {
    std::vector<voyx_t> input;
    std::vector<voyx_t> output;

    std::vector<voyx_t> frames;
    std::vector<std::complex<voyx_t>> dfts;
  }
  data;

  struct
  {
    std::vector<voyx_t> analysis;
    std::vector<voyx_t> synthesis;
  }
  windows;

  static void reject(const std::vector<size_t>& hops, const voyx::vector<voyx_t> input, voyx::matrix<voyx_t> frames, const std::vector<voyx_t>& window);
  static void inject(const std::vector<size_t>& hops, voyx::vector<voyx_t> output, const voyx::matrix<voyx_t> frames, const std::vector<voyx_t>& window);

};
