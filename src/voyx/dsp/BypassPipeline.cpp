#include <voyx/dsp/BypassPipeline.h>

#include <voyx/Source.h>

BypassPipeline::BypassPipeline(std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink) :
  SyncPipeline<voyx_t>(source, sink)
{
}

void BypassPipeline::operator()(const size_t index, const voyx::vector<voyx_t> input, voyx::vector<voyx_t> output)
{
  output = input;
}
