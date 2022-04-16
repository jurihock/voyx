#pragma once

#include <voyx/DSP/Pipeline.h>

#include <cmath>
#include <complex>
#include <numeric>
#include <vector>

class StftPipeline : public Pipeline<float>
{

public:

  StftPipeline(size_t framesize, size_t hopsize, std::shared_ptr<Source<float>> source, std::shared_ptr<Sink<float>> sink);

  void operator()(const size_t index, const std::vector<float>& input, std::vector<float>& output) override;

protected:

  virtual void operator()(std::vector<std::complex<float>>& dft) {}

private:

  const float PI2 = 2.0f * std::acos(-1.0f);

  const size_t framesize;
  const size_t hopsize;

  std::vector<size_t> hops;

  std::vector<float> frame;
  std::vector<std::complex<float>> dft;

  struct
  {
    std::vector<float> input;
    std::vector<float> output;
  }
  buffers;

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
