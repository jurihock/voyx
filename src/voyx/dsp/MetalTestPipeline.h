#ifdef VOYXMETAL

#pragma once

#include <voyx/Header.h>
#include <voyx/dsp/AsyncPipeline.h>

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>

class MetalTestPipeline : public AsyncPipeline<voyx_t>
{

public:

  MetalTestPipeline(const voyx_t samplerate, const size_t framesize, const size_t dftsize, std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink);

protected:

  const voyx_t samplerate;
  const size_t framesize;
  const size_t dftsize;

  void begin(const size_t index, const voyx::vector<voyx_t> input) override;

private:

};

#endif
