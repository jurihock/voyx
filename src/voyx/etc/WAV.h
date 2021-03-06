#pragma once

#include <voyx/Header.h>

struct WAV
{
  static void read(const std::string& path, std::vector<double>& data, const voyx_t samplerate);
  static void read(const std::string& path, std::vector<float>& data, const voyx_t samplerate);

  static void write(const std::string& path, const std::vector<double>& data, const voyx_t samplerate);
  static void write(const std::string& path, const std::vector<float>& data, const voyx_t samplerate);
};
