#pragma once

#include <voyx/Header.h>
#include <voyx/DSP/StftPipeline.h>
#include <voyx/UI/Plot.h>

class TestPipeline : public StftPipeline
{

public:

  TestPipeline(const size_t samplerate, const size_t framesize, const size_t hopsize, std::shared_ptr<Source<float>> source, std::shared_ptr<Sink<float>> sink, std::shared_ptr<Plot> plot);

  void operator()(const size_t index, const std::vector<float>& signal, const std::vector<std::span<std::complex<float>>>& dfts) override;

private:

  FFT<float> fft;

  std::shared_ptr<Plot> plot;

};
