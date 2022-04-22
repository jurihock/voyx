#pragma once

#include <voyx/Header.h>

namespace $$
{
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
}
