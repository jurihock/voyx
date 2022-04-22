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
    $$::interp<T>(x1, y1, x0, y0);
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

    const size_t N = x.size();
    const size_t M = static_cast<size_t>(std::round(N * factor));

    for (size_t m = 0; m < std::min(M, N); ++m)
    {
      const voyx_t n = m / factor;

      const ptrdiff_t n_ = static_cast<ptrdiff_t>(n);
      const ptrdiff_t n0 = n_ - (n < n_); // std::floor(n)
      const ptrdiff_t n1 = n_ + (n > n_); // std::ceil(n)

      if (n0 < 0 || N <= n0)
      {
        continue;
      }

      if (n1 < 0 || N <= n1)
      {
        continue;
      }

      if (n0 == n1)
      {
        y[m] = x[n0];
        continue;
      }

      const T y0 = x[n0];
      const T y1 = x[n1];

      const voyx_t i = (n - n0) / (n1 - n0);

      y[m] = y0 * (1 - i) + y1 * i;
    }
  }

  template<typename T>
  static inline void interp(const std::vector<T>& x, std::vector<T>& y, const voyx_t factor)
  {
    $$::interp<T>(x, y, factor);
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
