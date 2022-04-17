#include <voyx/DSP/BypassPipeline.h>

#include <voyx/Source.h>

BypassPipeline::BypassPipeline(std::shared_ptr<Source<float>> source, std::shared_ptr<Sink<float>> sink) :
  Pipeline(source, sink)
{
}

void BypassPipeline::operator()(const size_t index, const std::vector<float>& input, std::vector<float>& output)
{
  output = input;
}
