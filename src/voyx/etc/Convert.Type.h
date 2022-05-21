#pragma once

#include <voyx/Header.h>

namespace $$
{
  template<typename T>
  static inline std::string str()
  {
    return typeid(T).name();
  }

  template<typename T>
  static inline std::string str(const T value)
  {
    if constexpr (std::is_same<T, std::string>::value)
    {
      return value;
    }

    if constexpr (std::is_same<T, std::wstring>::value)
    {
      return std::string(value.begin(), value.end());
    }

    std::ostringstream result;

    if constexpr (std::is_same<T, bool>::value)
    {
      result << std::boolalpha << value;
    }
    else if constexpr (std::is_same<T, std::vector<float>>::value)
    {
      result << "[";
      for (size_t i = 0; i < value.size(); ++i)
      {
        if (i > 0)
        {
          result << ",";
        }

        result << value[i];
      }
      result << "]";
    }
    else if constexpr (std::is_same<T, std::vector<double>>::value)
    {
      result << "[";
      for (size_t i = 0; i < value.size(); ++i)
      {
        if (i > 0)
        {
          result << ",";
        }

        result << value[i];
      }
      result << "]";
    }
    else
    {
      result << value;
    }

    return result.str();
  }

  template<typename T>
  static inline T to(const std::string& value)
  {
    if (value.empty())
    {
      return T();
    }

    if constexpr (std::is_same<T, std::string>::value)
    {
      return value;
    }

    if constexpr (std::is_same<T, std::wstring>::value)
    {
      return std::wstring(value.begin(), value.end());
    }

    if constexpr (std::is_same<T, bool>::value)
    {
      bool boolean;
      std::istringstream(value)
        >> (std::isdigit(value.front()) ? std::noboolalpha : std::boolalpha)
        >> boolean;
      return boolean;
    }

    if constexpr (std::is_same<T, char>::value)
    {
      return value.front();
    }

    if constexpr (std::is_same<T, int>::value)
    {
      return std::stoi(value);
    }

    if constexpr (std::is_same<T, long>::value)
    {
      return std::stol(value);
    }

    if constexpr (std::is_same<T, size_t>::value)
    {
      return std::stoull(value);
    }

    if constexpr (std::is_same<T, float>::value)
    {
      return std::stof(value);
    }

    if constexpr (std::is_same<T, double>::value)
    {
      return std::stod(value);
    }

    if constexpr (std::is_same<T, std::vector<float>>::value)
    {
      const std::vector<std::string> strings =
        $$::split($$::erase(value, { '[', ']' }), ',');

      std::vector<float> floats(strings.size());

      for (size_t i = 0; i < strings.size(); ++i)
      {
        floats[i] = $$::to<float>(strings[i]);
      }

      return floats;
    }

    if constexpr (std::is_same<T, std::vector<double>>::value)
    {
      const std::vector<std::string> strings =
        $$::split($$::erase(value, { '[', ']' }), ',');

      std::vector<double> doubles(strings.size());

      for (size_t i = 0; i < strings.size(); ++i)
      {
        doubles[i] = $$::to<double>(strings[i]);
      }

      return doubles;
    }
  }
}
