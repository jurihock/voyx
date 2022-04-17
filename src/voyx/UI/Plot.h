#pragma once

#include <vector>

class Plot
{

public:

  virtual ~Plot() {}

  virtual void show() = 0;

  virtual void plot(const std::vector<float>& y) = 0;

};
