#pragma once

#include <voyx/Header.h>

namespace $$
{
  template<class value_getter_t, class T>
  size_t argmax(const voyx::vector<T> vector)
  {
    using value_t = typename $$::typeofvalue<T>::type;
    const value_getter_t getvalue;

    static_assert(std::is_arithmetic<value_t>::value);

    if (vector.empty())
    {
      return -1;
    }

    value_t value = getvalue(vector[0]);
    size_t index = 0;

    for (size_t i = 1; i < vector.size(); ++i)
    {
      if (getvalue(vector[i]) > value)
      {
        value = getvalue(vector[i]);
        index = i;
      }
    }

    return index;
  }

  template<class value_getter_t, class T>
  std::vector<size_t> argmax(const voyx::matrix<T> matrix, size_t axis = 0)
  {
    using value_t = typename $$::typeofvalue<T>::type;
    const value_getter_t getvalue;

    static_assert(std::is_arithmetic<value_t>::value);

    std::vector<size_t> indices;

    if (matrix.empty())
    {
      return indices;
    }

    const size_t shape[] =
    {
      matrix.size(),
      matrix.stride()
    };

    if (axis == 0)
    {
      indices.resize(shape[1]);

      for (size_t i = 0; i < shape[1]; ++i)
      {
        value_t value = getvalue(matrix(0, i));
        size_t index = 0;

        for (size_t j = 1; j < shape[0]; ++j)
        {
          if (getvalue(matrix(j, i)) > value)
          {
            value = getvalue(matrix(j, i));
            index = j;
          }
        }

        indices[i] = index;
      }
    }
    else if (axis == 1)
    {
      indices.resize(shape[0]);

      for (size_t i = 0; i < shape[0]; ++i)
      {
        value_t value = getvalue(matrix(i, 0));
        size_t index = 0;

        for (size_t j = 1; j < shape[1]; ++j)
        {
          if (getvalue(matrix(i, j)) > value)
          {
            value = getvalue(matrix(i, j));
            index = j;
          }
        }

        indices[i] = index;
      }
    }
    else
    {
      throw std::runtime_error("Invalid axis index!");
    }

    return indices;
  }
}
