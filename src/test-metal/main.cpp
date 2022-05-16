#include <iostream>
#include <filesystem>

#define NS_PRIVATE_IMPLEMENTATION
#include <Foundation/Foundation.hpp>

#define MTL_PRIVATE_IMPLEMENTATION
#include <Metal/Metal.hpp>

void test();

int main()
{
  test();

  return 0;
}

void test()
{
  NS::Error* error = nullptr;

  MTL::Device* device = MTL::CreateSystemDefaultDevice();

  if (device == nullptr)
  {
    throw std::runtime_error("Unable to create Metal device!");
  }

  auto filename = std::filesystem::current_path() / "build-release" / "test.metallib";
  auto filepath = NS::String::string(filename.c_str(), NS::ASCIIStringEncoding);

  error = nullptr;
  MTL::Library* library = device->newLibrary(filepath, &error);

  if (library == nullptr)
  {
    throw std::runtime_error("Unable to load Metal library!"); // TODO error
  }

  MTL::CommandQueue* queue = device->newCommandQueue();

  if (queue == nullptr)
  {
    throw std::runtime_error("Unable to create Metal queue!");
  }

  // SPECIFIC CODE

  MTL::CommandBuffer* command = queue->commandBuffer();

  if (command == nullptr)
  {
    throw std::runtime_error("Unable to create Metal command!");
  }

  MTL::ComputeCommandEncoder* encoder = command->computeCommandEncoder();

  if (encoder == nullptr)
  {
    throw std::runtime_error("Unable to create Metal encoder!");
  }

  auto add_arrays = NS::String::string("add_arrays", NS::ASCIIStringEncoding);

  MTL::Function* function = library->newFunction(add_arrays);

  if (function == nullptr)
  {
    throw std::runtime_error("Unable to find Metal function!");
  }

  error = nullptr;
  MTL::ComputePipelineState* pipe = device->newComputePipelineState(function, &error);

  if (pipe == nullptr)
  {
    throw std::runtime_error("Unable to create Metal pipeline!"); // TODO error
  }

  NS::UInteger size = 1000;
  NS::UInteger bytes = size * sizeof(float);

  MTL::Buffer* a = device->newBuffer(bytes, MTL::ResourceStorageModeShared);
  MTL::Buffer* b = device->newBuffer(bytes, MTL::ResourceStorageModeShared);
  MTL::Buffer* c = device->newBuffer(bytes, MTL::ResourceStorageModeShared);

  for (NS::UInteger i = 0; i < size; ++i)
  {
    ((float*)a->contents())[i] = i;
    ((float*)b->contents())[i] = i;
    ((float*)c->contents())[i] = 0;
  }

  encoder->setComputePipelineState(pipe);

  encoder->setBuffer(a, 0, 0);
  encoder->setBuffer(b, 0, 1);
  encoder->setBuffer(c, 0, 2);

  NS::UInteger threads = pipe->maxTotalThreadsPerThreadgroup();

  if (threads > size)
  {
    threads = size;
  }

  MTL::Size grid = MTL::Size::Make(size, 1, 1); // threads per grid
  MTL::Size group = MTL::Size::Make(threads, 1, 1); // threads per thread group

  encoder->dispatchThreads(grid, group);
  encoder->endEncoding();

  command->commit();
  command->waitUntilCompleted();

  for (NS::UInteger i = 0; i < size; ++i)
  {
    if (((float*)c->contents())[i] == i + i)
    {
      continue;
    }

    std::cout << i << std::endl;
  }
}
