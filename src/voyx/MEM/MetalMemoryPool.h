#pragma once

#include <voyx/Header.h>

#include <voyx/Metal.h>
#include <voyx/MEM/MemoryPool.h>

template<class T>
class MetalMemoryPool : public MemoryPool<T>
{

public:

  MetalMemoryPool() : MetalMemoryPool(MTL::CreateSystemDefaultDevice())
  {
  }

  MetalMemoryPool(MTL::Device* device) : device(device)
  {
    if (device == nullptr)
    {
      throw std::runtime_error("Invalid Metal device instance!");
    }
  }

  ~MetalMemoryPool()
  {
    if (device != nullptr)
    {
      // TODO how to delete?

      device = nullptr;
    }
  }

  voyx::vector<T> allocate(const size_t size) override
  {
    if (device == nullptr)
    {
      throw std::runtime_error("Invalid Metal device instance!");
    }

    MTL::Buffer* tag = device->newBuffer(size * sizeof(T), MTL::ResourceStorageModeShared);

    T* data = static_cast<T*>(tag->contents());

    voyx::vector<T> vector(data, size, tag);

    return vector;
  }

  void deallocate(voyx::vector<T> vector) override
  {
    if (vector.tag() != nullptr)
    {
      // TODO how to delete?

      vector.tag(nullptr);
    }
  }

private:

  MTL::Device* device;

};
