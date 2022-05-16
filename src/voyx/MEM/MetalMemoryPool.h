#pragma once

#include <voyx/Header.h>

#include <voyx/MEM/MemoryPool.h>

class MetalMemoryPool : public MemoryPool<float>
{

public:

  MetalMemoryPool();
  ~MetalMemoryPool();

  voyx::vector<float> allocate(const size_t size) override;
  void deallocate(voyx::vector<float> vector) override;

private:

  void* metal_device;

};
