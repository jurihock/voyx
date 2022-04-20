#pragma once

#include <voyx/Header.h>

template<typename T>
class Vocoder
{

public:

  Vocoder(const voyx_t samplerate, const size_t framesize, const size_t hopsize) :
    stft_freq_inc(samplerate / framesize),
    stft_phase_inc(PI2 * hopsize / framesize)
  {
    encode_phase_buffer.resize(framesize / 2 + 1);
    decode_phase_buffer.resize(framesize / 2 + 1);
  }

  void encode(const std::vector<std::span<std::complex<T>>>& dfts)
  {
    for (auto dft : dfts)
    {
      encode(dft);
    }
  }

  void decode(const std::vector<std::span<std::complex<T>>>& dfts)
  {
    for (auto dft : dfts)
    {
      decode(dft);
    }
  }

  void encode(std::span<std::complex<T>> dft)
  {
    T frequency,
      phase,
      delta,
      j;

    for (size_t i = 0; i < dft.size(); ++i)
    {
      phase = std::arg(dft[i]);

      delta = phase - encode_phase_buffer[i];
      encode_phase_buffer[i] = phase;

      j = wrap(delta - i * stft_phase_inc) / stft_phase_inc;

      frequency = (i + j) * stft_freq_inc;

      dft[i] = std::complex<T>(
        std::abs(dft[i]),
        frequency);
    }
  }

  void decode(std::span<std::complex<T>> dft)
  {
    T frequency,
      phase,
      delta,
      j;

    for (size_t i = 0; i < dft.size(); ++i)
    {
      frequency = dft[i].imag();

      j = (frequency - i * stft_freq_inc) / stft_freq_inc;

      delta = (i + j) * stft_phase_inc;

      decode_phase_buffer[i] += delta;
      phase = decode_phase_buffer[i];

      dft[i] = std::polar<T>(
        dft[i].real(),
        phase);
    }
  }

private:

  const T PI2 = T(2) * std::acos(T(-1));

  const T stft_freq_inc;
  const T stft_phase_inc;

  std::vector<T> encode_phase_buffer;
  std::vector<T> decode_phase_buffer;

  inline T wrap(const T phase) const
  {
    return phase - PI2 * std::floor(phase / PI2 + T(0.5));
  }

};
