#pragma once

#include <voyx/Header.h>
#include <voyx/alg/FFT.h>

/**
 * Short-Time Fourier Transform implementation.
 **/
template <typename T, typename F>
class STFT
{

public:

  STFT(const size_t framesize, const size_t hopsize) :
    framesize(framesize),
    hopsize(hopsize),
    fft(framesize)
  {
    const std::vector<F> window = Window<F>(framesize);

    windows.analysis = window;
    windows.synthesis = window;

    const F unitygain = hopsize / std::inner_product(
      windows.synthesis.begin(), windows.synthesis.end(), windows.synthesis.begin(), F(0));

    std::transform(windows.synthesis.begin(), windows.synthesis.end(), windows.synthesis.begin(),
      [unitygain](F value) { return value * unitygain; });

    for (size_t hop = 0; (hop + framesize) < (framesize * 2); hop += hopsize)
    {
      data.hops.push_back(hop);
    }

    data.input.resize(framesize * 2);
    data.output.resize(framesize * 2);
    data.frames.resize(framesize * data.hops.size());
  }

  size_t size() const
  {
    return fft.dftsize();
  }

  const std::vector<size_t>& hops() const
  {
    return data.hops;
  }

  const voyx::vector<T> signal() const
  {
    return data.input;
  }

  void stft(const voyx::vector<T> samples, voyx::matrix<std::complex<F>> dfts)
  {
    for (size_t i = 0; i < framesize; ++i)
    {
      const size_t j = i + framesize;

      data.input[i] = data.input[j];
      data.input[j] = samples[i];

      data.output[i] = data.output[j];
      data.output[j] = 0;
    }

    voyx::matrix<F> frames(data.frames, framesize);

    reject(frames, data.input, data.hops, windows.analysis);

    fft.fft(frames, dfts);
  }

  void istft(const voyx::matrix<std::complex<F>> dfts, voyx::vector<T> samples)
  {
    voyx::matrix<F> frames(data.frames, framesize);

    fft.ifft(dfts, frames);

    inject(frames, data.output, data.hops, windows.synthesis);

    for (size_t i = 0; i < framesize; ++i)
    {
      samples[i] = data.output[i];
    }
  }

private:

  const size_t framesize;
  const size_t hopsize;

  const FFT<F> fft;

  struct
  {
    std::vector<F> analysis;
    std::vector<F> synthesis;
  }
  windows;

  struct
  {
    std::vector<T> input;
    std::vector<T> output;
    std::vector<F> frames;
    std::vector<size_t> hops;
  }
  data;

  static void reject(voyx::matrix<F> frames, const voyx::vector<T> input, const std::vector<size_t>& hops, const std::vector<F>& window)
  {
    for (size_t i = 0; i < hops.size(); ++i)
    {
      const auto hop = hops[i];
      auto frame = frames[i];

      for (size_t j = 0; j < window.size(); ++j)
      {
        frame[j] = input[hop + j] * window[j];
      }
    }
  }

  static void inject(const voyx::matrix<F> frames, voyx::vector<T> output, const std::vector<size_t>& hops, const std::vector<F>& window)
  {
    for (size_t i = 0; i < hops.size(); ++i)
    {
      const auto hop = hops[i];
      const auto frame = frames[i];

      for (size_t j = 0; j < window.size(); ++j)
      {
        output[hop + j] += frame[j] * window[j];
      }
    }
  }

};
