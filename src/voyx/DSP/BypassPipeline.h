#pragma once

#include <voyx/Header.h>
#include <voyx/DSP/Pipeline.h>

class BypassPipeline : public Pipeline<voyx_t>
{

public:

  BypassPipeline(std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink);

  void operator()(const size_t index, const voyx::vector<voyx_t> input, voyx::vector<voyx_t> output) override;

};
