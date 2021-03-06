#pragma once

#include <voyx/Header.h>
#include <voyx/dsp/SyncPipeline.h>

class BypassPipeline : public SyncPipeline<voyx_t>
{

public:

  BypassPipeline(std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink);

protected:

  void operator()(const size_t index, const voyx::vector<voyx_t> input, voyx::vector<voyx_t> output) override;

};
