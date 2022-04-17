#pragma once

#include <voyx/Header.h>

struct WAV
{
  static void read(const std::string& path, std::vector<float>& data, const size_t samplerate);
  static void write(const std::string& path, const std::vector<float>& data, const size_t samplerate);
};
