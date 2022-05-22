#ifdef VOYXOPENCL

#include <voyx/dsp/OpenclTestPipeline.h>

#include <voyx/Source.h>

OpenclTestPipeline::OpenclTestPipeline(const voyx_t samplerate, const size_t framesize, const size_t dftsize, std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink) :
  SyncPipeline<voyx_t>(source, sink),
  samplerate(samplerate),
  framesize(framesize),
  dftsize(dftsize)
{
  const cl_device_type type = CL_DEVICE_TYPE_GPU;

  std::vector<cl::Device> devices;

  std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);

  for (const auto& platform : platforms)
  {
    std::vector<cl::Device> subdevices;

    platform.getDevices(type, &subdevices);

    for (const auto& subdevice : subdevices)
    {
      devices.push_back(subdevice);
    }
  }

  for (const auto& device : devices)
  {
    const std::string name = device.getInfo<CL_DEVICE_NAME>();
    const std::string version = device.getInfo<CL_DEVICE_VERSION>();
    const std::string compiler = device.getInfo<CL_DEVICE_OPENCL_C_VERSION>();

    LOG(INFO) << $("OpenCL device {0} ({1}, {2})", name, version, compiler);
  }

  voyxassert(devices.size() > 0);

  device = devices.front();
  context = cl::Context(device);
  queue = cl::CommandQueue(context, device);
  program = cl::Program(context, kernels);

  try
  {
    program.build(device);
  }
  catch (...)
  {
    LOG(INFO) << "Unable to build GPU kernels!";

    cl_int error = CL_SUCCESS;

    const auto logs = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(&error);

    for (const auto& log : logs)
    {
      LOG(ERROR) << log.second;
    }
  }

  sdft1 = cl::Kernel(program, "sdft1");
  sdft2 = cl::Kernel(program, "sdft2");

  buffer.inputs.resize(dftsize);
  buffer.outputs.resize((dftsize + 2) * framesize);
  buffer.dfts.resize(dftsize * framesize);
  buffer.cursor = 0;
}

#define CLBUFFER(vector, type) cl::Buffer(context, CL_MEM_USE_HOST_PTR, vector.size() * sizeof(type), (void*)vector.data());

void OpenclTestPipeline::operator()(const size_t index, const voyx::vector<voyx_t> input, voyx::vector<voyx_t> output)
{
  cl::Event event;
  {
    cl::Buffer samples = CLBUFFER(input, voyx_t);
    cl::Buffer inputs = CLBUFFER(buffer.inputs, voyx_t);
    cl::Buffer outputs = CLBUFFER(buffer.outputs, std::complex<voyx_t>);

    sdft1.setArg<cl::Buffer>(0, samples);
    sdft1.setArg<cl::Buffer>(1, inputs);
    sdft1.setArg<cl::Buffer>(2, outputs);

    sdft1.setArg<int>(3, framesize);
    sdft1.setArg<int>(4, dftsize);
    sdft1.setArg<int>(5, buffer.cursor);

    queue.enqueueNDRangeKernel(sdft1, cl::NullRange, cl::NDRange(dftsize), cl::NullRange);
  }
  {
    cl::Buffer samples = CLBUFFER(input, voyx_t);
    cl::Buffer inputs = CLBUFFER(buffer.inputs, voyx_t);
    cl::Buffer outputs = CLBUFFER(buffer.outputs, std::complex<voyx_t>);
    cl::Buffer dfts = CLBUFFER(buffer.dfts, std::complex<voyx_t>);

    sdft2.setArg<cl::Buffer>(0, outputs);
    sdft2.setArg<cl::Buffer>(1, dfts);

    sdft2.setArg<int>(2, dftsize);
    sdft2.setArg<voyx_t>(3, voyx_t(1) / dftsize);

    queue.enqueueNDRangeKernel(sdft2, cl::NullRange, cl::NDRange(dftsize, framesize), cl::NullRange, nullptr, &event);
  }

  event.wait();

  voyx::matrix<std::complex<voyx_t>> dfts(buffer.dfts, dftsize);

  for (size_t i = 0; i < framesize; ++i)
  {
    const voyx::vector<std::complex<voyx_t>> dft = dfts[i];

    voyx_t sample = 0;

    for (size_t j = 0; j < dftsize; ++j)
    {
      sample += dft[j].real() * (j % 2 ? -1 : +1);
    }

    output[i] = sample;
  }

  size_t cursor = buffer.cursor;

  for (size_t i = 0; i < framesize; ++i)
  {
    buffer.inputs[cursor] = input[i];

    if (++cursor >= buffer.inputs.size())
    {
      cursor = 0;
    }
  }

  buffer.cursor = cursor;
}

#endif
