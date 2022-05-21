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

  bypass = cl::Kernel(program, "bypass");
}

void OpenclTestPipeline::operator()(const size_t index, const voyx::vector<voyx_t> input, voyx::vector<voyx_t> output)
{
  cl::Buffer x = cl::Buffer(context, CL_MEM_USE_HOST_PTR, framesize * sizeof(voyx_t), (void*)input.data());
  cl::Buffer y = cl::Buffer(context, CL_MEM_USE_HOST_PTR, framesize * sizeof(voyx_t), (void*)output.data());

  bypass.setArg<cl::Buffer>(0, x);
  bypass.setArg<cl::Buffer>(1, y);

  cl::Event event;

  queue.enqueueNDRangeKernel(bypass, cl::NullRange, cl::NDRange(framesize), cl::NullRange, nullptr, &event);

  event.wait();
}

#endif
