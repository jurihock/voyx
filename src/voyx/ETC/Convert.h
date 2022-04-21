#pragma once

#include <voyx/Header.h>

#include <mlinterp/mlinterp.hpp>

namespace $$::dft
{
  template<typename T>
  static inline T bin(const T freq, const voyx_t samplerate, const size_t framesize)
  {
    return static_cast<T>(
      freq * framesize / samplerate);
  }

  template<typename T>
  static inline T freq(const T bin, const voyx_t samplerate, const size_t framesize)
  {
    return static_cast<T>(
      bin * samplerate / framesize);
  }

  template<typename T>
  static inline std::vector<T> bins(const size_t framesize)
  {
    std::vector<T> values(framesize / 2 + 1);
    std::iota(values.begin(), values.end(), 0);
    return values;
  }

  template<typename T>
  static inline std::vector<T> freqs(const voyx_t samplerate, const size_t framesize)
  {
    std::vector<T> values(framesize / 2 + 1);
    std::iota(values.begin(), values.end(), 0);
    std::transform(values.begin(), values.end(), values.begin(),
      [samplerate, framesize](T i) { return $$::dft::freq(i, samplerate, framesize); });
    return values;
  }
}

namespace $$::midi
{
  template<typename T>
  static inline T key(const T freq, const voyx_t concertpitch)
  {
    return static_cast<T>(
      (freq > 0) ? 69 + 12 * std::log2(freq / concertpitch) : 0);
  }

  template<typename T>
  static inline T freq(const T key, const voyx_t concertpitch)
  {
    return static_cast<T>(
      std::pow(2, (key - 69) / 12) * concertpitch);
  }

  template<typename T>
  static inline std::vector<T> keys(const std::vector<T>& freqs, const voyx_t concertpitch)
  {
    std::vector<T> values(freqs.size());
    std::transform(freqs.begin(), freqs.end(), values.begin(),
      [concertpitch](voyx_t i) { return $$::midi::key(i, concertpitch); });
    return values;
  }

  template<typename T>
  static inline std::vector<T> keys()
  {
    std::vector<T> values(128);
    std::iota(values.begin(), values.end(), 0);
    return values;
  }

  template<typename T>
  static inline std::vector<T> freqs(const voyx_t concertpitch)
  {
    std::vector<T> values(128);
    std::iota(values.begin(), values.end(), 0);
    std::transform(values.begin(), values.end(), values.begin(),
      [concertpitch](T i) { return $$::midi::freq(i, concertpitch); });
    return values;
  }
}

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

      const voyx_t n0 = std::floor(n);
      const voyx_t n1 = std::ceil(n);

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
        y[m] = x[static_cast<size_t>(n0)];
        continue;
      }

      const T y0 = x[static_cast<size_t>(n0)];
      const T y1 = x[static_cast<size_t>(n1)];

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

  static inline bool match(const std::string& value, const std::string& pattern)
  {
    const auto regex = std::regex(pattern);

    return std::regex_match(value, regex);
  }

  static inline bool imatch(const std::string& value, const std::string& pattern)
  {
    const auto regex = std::regex(pattern, std::regex_constants::icase);

    return std::regex_match(value, regex);
  }

  static inline std::string lower(const std::string& value)
  {
    std::string result(value);

    std::transform(result.begin(), result.end(), result.begin(),
      [](int c) { return std::tolower(c); });

    return result;
  }

  static inline std::string upper(const std::string& value)
  {
    std::string result(value);

    std::transform(result.begin(), result.end(), result.begin(),
      [](int c) { return std::toupper(c); });

    return result;
  }

  static inline std::string trim(const std::string& value)
  {
    std::string result(value);

    result.erase(result.begin(), std::find_if(result.begin(), result.end(),
      [](int c) { return !std::isspace(c); }));

    result.erase(std::find_if(result.rbegin(), result.rend(),
      [](int c) { return !std::isspace(c); }).base(), result.end());

    return result;
  }

  static inline std::string erase(const std::string& value, const char what)
  {
    std::string result(value);

    result.erase(std::remove(result.begin(), result.end(), what), result.end());

    return result;
  }

  static inline std::string erase(const std::string& value, const std::vector<char>& what)
  {
    std::string result(value);

    for (size_t i = 0; i < what.size(); ++i)
    {
      result.erase(std::remove(result.begin(), result.end(), what[i]), result.end());
    }

    return result;
  }

  static inline std::string replace(const std::string& value, char what, char with)
  {
    std::string result(value);

    std::replace(result.begin(), result.end(), what, with);

    return result;
  }

  static inline std::string replace(const std::string& value, const std::string& what, const std::string& with)
  {
    if (value.empty() || what.empty())
    {
      return value;
    }

    std::string result(value);

    size_t cursor = 0;

    while ((cursor = result.find(what, cursor)) != std::string::npos)
    {
      result.replace(cursor, what.length(), with);
      cursor += with.length();
    }

    return result;
  }

  static inline std::vector<std::string> split(const std::string& value, const char delimiter)
  {
    std::vector<std::string> result;

    std::stringstream lines(value);
    std::string line;

    while (std::getline(lines, line, delimiter))
    {
      result.push_back(line);
    }

    return result;
  }

  static inline std::string join(const std::vector<std::string>& values, const std::string& delimiter)
  {
    if (values.empty())
    {
      return "";
    }

    if (values.size() == 1)
    {
      return values.front();
    }

    std::ostringstream result;

    result << values.front();

    for (size_t i = 1; i < values.size(); ++i)
    {
      result << delimiter << values.at(i);
    }

    return result.str();
  }

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
