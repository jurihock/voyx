#pragma once

#include <voyx/Header.h>
#include <voyx/DSP/Pipeline.h>
#include <voyx/UI/Plot.h>

class TestPipeline : public Pipeline<float>
{

public:

  TestPipeline(std::shared_ptr<Plot> plot, std::shared_ptr<Source<float>> source, std::shared_ptr<Sink<float>> sink);

  void operator()(const size_t index, const std::vector<float>& input, std::vector<float>& output) override;

private:

  std::shared_ptr<Plot> plot;

};
