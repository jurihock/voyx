#include <voyx/DSP/BypassPipeline.h>

#include <voyx/Source.h>

BypassPipeline::BypassPipeline(std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink) :
  Pipeline(source, sink)
{
}

void BypassPipeline::operator()(const size_t index, const std::vector<voyx_t>& input, std::vector<voyx_t>& output)
{
  output = input;
}
