#pragma once

#include <voyx/Header.h>
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

  virtual void operator()(const size_t index, const std::vector<float>& signal, std::vector<std::vector<std::complex<float>>>& dfts) = 0;

private:

  std::vector<size_t> hops;

  struct
  {
    std::vector<float> input;
    std::vector<float> output;
    std::vector<std::vector<float>> frames;
    std::vector<std::vector<std::complex<float>>> dfts;
  }
  data;

  struct
  {
    std::vector<float> analysis;
    std::vector<float> synthesis;
  }
  windows;

  static void reject(const size_t hop, const float* input, std::vector<float>& frame, const std::vector<float>& window);
  static void inject(const size_t hop, float* const output, const std::vector<float>& frame, const std::vector<float>& window);

  static void fft(const std::vector<float>& frame, std::vector<std::complex<float>>& dft);
  static void ifft(const std::vector<std::complex<float>>& dft, std::vector<float>& frame);

};
