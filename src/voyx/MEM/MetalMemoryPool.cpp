#include <voyx/Source.h>

#include <voyx/MEM/MetalMemoryPool.h>

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>

MetalMemoryPool::MetalMemoryPool() :
  metal_device(nullptr)
{
  MTL::Device* device = MTL::CreateSystemDefaultDevice();

  if (device == nullptr)
  {
    throw std::runtime_error("Unable to create Metal device!");
  }

  metal_device = device;
}

MetalMemoryPool::~MetalMemoryPool()
{
  if (metal_device != nullptr)
  {
    // TODO how to delete?

    metal_device = nullptr;
  }
}

voyx::vector<float> MetalMemoryPool::allocate(const size_t size)
{
  MTL::Device* device = static_cast<MTL::Device*>(metal_device);

  MTL::Buffer* tag = device->newBuffer(size * sizeof(float), MTL::ResourceStorageModeShared);

  float* data = static_cast<float*>(tag->contents());

  voyx::vector<float> vector(data, size, tag);

  return vector;
}

void MetalMemoryPool::deallocate(voyx::vector<float> vector)
{
  if (vector.tag() != nullptr)
  {
    // TODO how to delete?

    vector.tag(nullptr);
  }
}
