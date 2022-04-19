#pragma once

#include <voyx/Header.h>
#include <voyx/ETC/FFT.h>
#include <voyx/ETC/Window.h>
#include <voyx/DSP/Pipeline.h>

class StftPipeline : public Pipeline<voyx_t>
{

public:

  StftPipeline(const size_t samplerate, const size_t framesize, const size_t hopsize, std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink);

  void operator()(const size_t index, const std::vector<voyx_t>& input, std::vector<voyx_t>& output) override;

protected:

  const size_t samplerate;
  const size_t framesize;
  const size_t hopsize;

  #ifdef _OPENMP
  const bool parallelize = false; // temporary disabled to prevent warmup
  #else
  const bool parallelize = false;
  #endif

  void warmup() override;

  virtual void operator()(const size_t index, const std::vector<voyx_t>& signal, const std::vector<std::span<std::complex<voyx_t>>>& dfts) = 0;

private:

  const FFT<voyx_t> fft;

  std::vector<size_t> hops;

  struct
  {
    std::vector<voyx_t> input;
    std::vector<voyx_t> output;

    std::vector<voyx_t> frames;
    std::vector<std::complex<voyx_t>> dfts;

    struct
    {
      std::vector<std::span<voyx_t>> frames;
      std::vector<std::span<std::complex<voyx_t>>> dfts;
    }
    views;
  }
  data;

  struct
  {
    std::vector<voyx_t> analysis;
    std::vector<voyx_t> synthesis;
  }
  windows;

  static void reject(const std::vector<size_t>& hops, const std::vector<voyx_t>& input, const std::vector<std::span<voyx_t>>& frames, const std::vector<voyx_t>& window);
  static void inject(const std::vector<size_t>& hops, std::vector<voyx_t>& output, const std::vector<std::span<voyx_t>>& frames, const std::vector<voyx_t>& window);

};
