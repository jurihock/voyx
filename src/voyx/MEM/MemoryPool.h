#pragma once

#include <voyx/Header.h>

template<class T>
class MemoryPool
{

public:

  virtual ~MemoryPool() {}

  virtual voyx::vector<T> allocate(const size_t size) = 0;
  virtual void deallocate(voyx::vector<T> vector) = 0;

};
