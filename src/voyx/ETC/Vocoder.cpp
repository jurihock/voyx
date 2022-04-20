#include <voyx/ETC/Vocoder.h>

#include <voyx/Source.h>

Vocoder::Vocoder(const voyx_t samplerate, const size_t framesize, const size_t hopsize) :
  stft_freq_inc(samplerate / framesize),
  stft_phase_inc(PI2 * hopsize / framesize)
{
  encode_phase_buffer.resize(framesize / 2 + 1);
  decode_phase_buffer.resize(framesize / 2 + 1);
}

void Vocoder::encode(const std::vector<std::span<std::complex<voyx_t>>>& dfts)
{
  for (auto dft : dfts)
  {
    encode(dft);
  }
}

void Vocoder::decode(const std::vector<std::span<std::complex<voyx_t>>>& dfts)
{
  for (auto dft : dfts)
  {
    decode(dft);
  }
}

void Vocoder::encode(std::span<std::complex<voyx_t>> dft)
{
  voyx_t frequency,
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

    dft[i] = std::complex<voyx_t>(
      std::abs(dft[i]),
      frequency);
  }
}

void Vocoder::decode(std::span<std::complex<voyx_t>> dft)
{
  voyx_t frequency,
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

    dft[i] = std::polar<voyx_t>(
      dft[i].real(),
      phase);
  }
}
