#ifdef VOYXOPENCL

#pragma once

#include <voyx/Header.h>
#include <voyx/dsp/SyncPipeline.h>

#include <CL/opencl.hpp>

class OpenclTestPipeline : public SyncPipeline<voyx_t>
{

public:

  OpenclTestPipeline(const voyx_t samplerate, const size_t framesize, const size_t dftsize, std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink);

protected:

  const voyx_t samplerate;
  const size_t framesize;
  const size_t dftsize;

  void operator()(const size_t index, const voyx::vector<voyx_t> input, voyx::vector<voyx_t> output) override;

private:

  cl::Device device;
  cl::Context context;
  cl::CommandQueue queue;
  cl::Program program;
  cl::Kernel bypass;

const std::string kernels =
  R"(

    kernel void bypass(global const float* input,
                       global float* output)
    {
      const int i = get_global_id(0);

      output[i] = input[i];
    }

  )";

};

#endif
