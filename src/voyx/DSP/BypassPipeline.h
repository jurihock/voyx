#pragma once

#include <voyx/DSP/Pipeline.h>

class BypassPipeline : public Pipeline<float>
{

public:

  BypassPipeline(std::shared_ptr<Source<float>> source, std::shared_ptr<Sink<float>> sink);

  void operator()(const size_t index, const std::vector<float>& input, std::vector<float>& output) override;

};
