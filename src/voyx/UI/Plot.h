#pragma once

#include <voyx/Header.h>

class Plot
{

public:

  virtual ~Plot() {}

  virtual void show() = 0;

  virtual void xlim(const double min, const double max) = 0;
  virtual void ylim(const double min, const double max) = 0;

  virtual void xrange(const double max) = 0;
  virtual void xrange(const double min, const double max) = 0;

  virtual void plot(const std::vector<float>& y) = 0;
  virtual void plot(const std::vector<double>& y) = 0;

};
