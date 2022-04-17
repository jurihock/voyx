#pragma once

#include <voyx/Header.h>
#include <voyx/DSP/StftPipeline.h>
#include <voyx/UI/Plot.h>

class TestPipeline : public StftPipeline
{

public:

  TestPipeline(const size_t samplerate, const size_t framesize, const size_t hopsize, std::shared_ptr<Plot> plot, std::shared_ptr<Source<float>> source, std::shared_ptr<Sink<float>> sink);

  void operator()(std::vector<std::vector<std::complex<float>>>& dfts) override;

private:

  std::shared_ptr<Plot> plot;

};
