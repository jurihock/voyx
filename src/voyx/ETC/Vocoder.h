#pragma once

#include <voyx/Header.h>

class Vocoder
{

public:

  Vocoder(const voyx_t samplerate, const size_t framesize, const size_t hopsize);

  void encode(const std::vector<std::span<std::complex<voyx_t>>>& dfts);
  void decode(const std::vector<std::span<std::complex<voyx_t>>>& dfts);

  void encode(std::span<std::complex<voyx_t>> dft);
  void decode(std::span<std::complex<voyx_t>> dft);

private:

  const voyx_t PI2 = voyx_t(2) * std::acos(voyx_t(-1));

  const voyx_t stft_freq_inc;
  const voyx_t stft_phase_inc;

  std::vector<voyx_t> encode_phase_buffer;
  std::vector<voyx_t> decode_phase_buffer;

  inline voyx_t wrap(const voyx_t phase) const
  {
    return phase - PI2 * std::floor(phase / PI2 + voyx_t(0.5));
  }

};
