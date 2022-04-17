#include <voyx/DSP/TestPipeline.h>

#include <voyx/Source.h>

TestPipeline::TestPipeline(std::shared_ptr<Plot> plot, std::shared_ptr<Source<float>> source, std::shared_ptr<Sink<float>> sink) :
  Pipeline(source, sink),
  plot(plot)
{
}

void TestPipeline::operator()(const size_t index, const std::vector<float>& input, std::vector<float>& output)
{
  if (plot != nullptr)
  {
    plot->plot(input);
  }

  output = input;
}
