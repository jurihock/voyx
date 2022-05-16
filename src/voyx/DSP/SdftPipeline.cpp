#include <voyx/DSP/SdftPipeline.h>

#include <voyx/Source.h>

// TODO METAL
#define NULLERROR(obj, msg) if ((obj) == nullptr) { throw std::runtime_error((msg)); }
#include <voyx/Metal.h>
NS::Error* error;
MTL::Device* device;
MTL::Library* library;
MTL::CommandQueue* queue;
MTL::Function* bypass;
MTL::ComputePipelineState* pipeline;
MTL::Size grid;
MTL::Size group;
MTL::Buffer* x;
MTL::Buffer* y;

SdftPipeline::SdftPipeline(const voyx_t samplerate, const size_t framesize, const size_t dftsize, std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink) :
  Pipeline(source, sink),
  samplerate(samplerate),
  framesize(framesize),
  dftsize(dftsize),
  sdft(dftsize)
{
  data.dfts.resize(framesize * dftsize);

  // TODO METAL
  {
    device = MTL::CreateSystemDefaultDevice();
    NULLERROR(device, "Unable to create Metal device!");

    library = device->newDefaultLibrary();
    NULLERROR(library, "Unable to load Metal library!");

    queue = device->newCommandQueue();
    NULLERROR(queue, "Unable to create Metal command queue!");

    const auto name = NS::String::string("bypass", NS::ASCIIStringEncoding);
    bypass = library->newFunction(name);
    NULLERROR(bypass, "Unable to find Metal function!");

    pipeline = device->newComputePipelineState(bypass, &error);
    NULLERROR(pipeline, "Unable to create Metal pipeline!");

    NS::UInteger threads = pipeline->maxTotalThreadsPerThreadgroup();

    if (threads > framesize)
    {
      threads = framesize;
    }

    grid = MTL::Size::Make(framesize, 1, 1); // threads per grid
    group = MTL::Size::Make(threads, 1, 1); // threads per thread group

    x = device->newBuffer(framesize * sizeof(voyx_t), MTL::ResourceStorageModeShared);
    y = device->newBuffer(framesize * sizeof(voyx_t), MTL::ResourceStorageModeShared);
  }
}

void SdftPipeline::operator()(const size_t index, const voyx::vector<voyx_t> input, voyx::vector<voyx_t> output)
{
  voyx::matrix<std::complex<voyx_t>> dfts(data.dfts, dftsize);

  // TODO METAL
  {
    std::memcpy(x->contents(), input.data(), framesize * sizeof(voyx_t));

    MTL::CommandBuffer* command = queue->commandBuffer();
    NULLERROR(command, "Unable to create Metal command buffer!");

    MTL::ComputeCommandEncoder* encoder = command->computeCommandEncoder();
    NULLERROR(encoder, "Unable to create Metal command encoder!");

    encoder->setComputePipelineState(pipeline);
    encoder->setBuffer(x, 0, 0);
    encoder->setBuffer(y, 0, 1);

    encoder->dispatchThreads(grid, group);
    encoder->endEncoding();

    command->commit();
    command->waitUntilCompleted();

    std::memcpy(output.data(), y->contents(), framesize * sizeof(voyx_t));
  }

  // sdft.sdft(input, dfts);
  // (*this)(index, dfts);
  // sdft.isdft(dfts, output);
}
