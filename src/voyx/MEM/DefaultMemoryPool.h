#pragma once

#include <voyx/Header.h>

#include <voyx/MEM/MemoryPool.h>

template<class T>
class DefaultMemoryPool : public MemoryPool<T>
{

public:

  voyx::vector<T> allocate(const size_t size) override
  {
    auto tag = new std::vector<T>(size);

    voyx::vector<T> vector(tag->data(), tag->size(), tag);

    return vector;
  }

  void deallocate(voyx::vector<T> vector) override
  {
    if (vector.tag() != nullptr)
    {
      auto tag = static_cast<std::vector<T>*>(vector.tag());

      delete tag;

      vector.tag(nullptr);
    }
  }

};
