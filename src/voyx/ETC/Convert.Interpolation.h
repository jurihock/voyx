#pragma once

#include <voyx/Header.h>

#include <mlinterp/mlinterp.hpp>

namespace $$
{
  template<typename T>
  static inline void interp(const std::span<const T> x1, std::span<T> y1, const std::span<const T> x0, const std::span<const T> y0)
  {
    assert(x0.size() == y0.size());
    assert(x1.size() == y1.size());

    const size_t n0[] = { x0.size() };
    const size_t n1 = x1.size();

    mlinterp::interp(
      n0,        n1,
      y0.data(), y1.data(),
      x0.data(), x1.data());
  }

  template<typename T>
  static inline void interp(const std::vector<T>& x1, std::vector<T>& y1, const std::vector<T>& x0, const std::vector<T>& y0)
  {
    assert(x0.size() == y0.size());
    assert(x1.size() == y1.size());

    const size_t n0[] = { x0.size() };
    const size_t n1 = x1.size();

    mlinterp::interp(
      n0,        n1,
      y0.data(), y1.data(),
      x0.data(), x1.data());
  }

  template<typename T>
  static inline std::vector<T> interp(const std::span<const T> x1, const std::span<const T> x0, const std::span<const T> y0)
  {
    std::vector<T> y1(x1.size());
    $$::interp<T>(x1, y1, x0, y0);
    return y1;
  }

  template<typename T>
  static inline std::vector<T> interp(const std::vector<T>& x1, const std::vector<T>& x0, const std::vector<T>& y0)
  {
    std::vector<T> y1(x1.size());
    $$::interp<T>(x1, y1, x0, y0);
    return y1;
  }

  template<typename T>
  static inline void interp(const std::span<const T> x, std::span<T> y, const voyx_t factor)
  {
    assert(x.size() == y.size());

    if (factor == 1)
    {
      y = x;
      return;
    }

    const ptrdiff_t n = static_cast<ptrdiff_t>(x.size());
    const ptrdiff_t m = static_cast<ptrdiff_t>(n * factor);

    const T q = T(n) / T(m);

    for (ptrdiff_t i = 0; i < std::min(n, m); ++i)
    {
      T k = i * q;

      const ptrdiff_t j = static_cast<ptrdiff_t>(std::trunc(k));

      k = k - j;

      const bool ok = (0 <= j) && (j < n - 1);

      if (!ok)
      {
        continue;
      }

      y[i] = k * x[j + 1] + (1 - k) * x[j];
    }
  }

  template<typename T>
  static inline void interp(const std::vector<T>& x, std::vector<T>& y, const voyx_t factor)
  {
    assert(x.size() == y.size());

    if (factor == 1)
    {
      y = x;
      return;
    }

    const ptrdiff_t n = static_cast<ptrdiff_t>(x.size());
    const ptrdiff_t m = static_cast<ptrdiff_t>(n * factor);

    const T q = T(n) / T(m);

    for (ptrdiff_t i = 0; i < std::min(n, m); ++i)
    {
      T k = i * q;

      const ptrdiff_t j = static_cast<ptrdiff_t>(std::trunc(k));

      k = k - j;

      const bool ok = (0 <= j) && (j < n - 1);

      if (!ok)
      {
        continue;
      }

      y[i] = k * x[j + 1] + (1 - k) * x[j];
    }
  }

  template<typename T>
  static inline std::vector<T> interp(const std::span<const T> x, const voyx_t factor)
  {
    std::vector<T> y(x.size());
    $$::interp<T>(x, y, factor);
    return y;
  }

  template<typename T>
  static inline std::vector<T> interp(const std::vector<T>& x, const voyx_t factor)
  {
    std::vector<T> y(x.size());
    $$::interp<T>(x, y, factor);
    return y;
  }
}
