#pragma once

#include <voyx/Header.h>
#include <voyx/ETC/FFT.h>
#include <voyx/ETC/Window.h>
#include <voyx/DSP/Pipeline.h>

class StftPipeline : public Pipeline<float>
{

public:

  StftPipeline(const size_t samplerate, const size_t framesize, const size_t hopsize, std::shared_ptr<Source<float>> source, std::shared_ptr<Sink<float>> sink);

  void operator()(const size_t index, const std::vector<float>& input, std::vector<float>& output) override;

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

  virtual void operator()(const size_t index, const std::vector<float>& signal, const std::vector<std::span<std::complex<float>>>& dfts) = 0;

private:

  const FFT<float> fft;

  std::vector<size_t> hops;

  struct
  {
    std::vector<float> input;
    std::vector<float> output;

    std::vector<float> frames;
    std::vector<std::complex<float>> dfts;

    struct
    {
      std::vector<std::span<float>> frames;
      std::vector<std::span<std::complex<float>>> dfts;
    }
    views;
  }
  data;

  struct
  {
    std::vector<float> analysis;
    std::vector<float> synthesis;
  }
  windows;

  static void reject(const size_t hop, const std::vector<float>& input, const std::span<float>& frame, const std::vector<float>& window);
  static void inject(const size_t hop, std::vector<float>& output, const std::span<const float>& frame, const std::vector<float>& window);

};
