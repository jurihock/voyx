#include <voyx/DSP/StftPipeline.h>

#include <voyx/Voyx.h>

#include <pocketfft/pocketfft_hdronly.h>

StftPipeline::StftPipeline(size_t framesize, size_t hopsize, std::shared_ptr<Source<float>> source, std::shared_ptr<Sink<float>> sink) :
  Pipeline(source, sink),
  framesize(framesize),
  hopsize(hopsize),
  frame(framesize),
  dft(framesize / 2 + 1)
{
  if (source->framesize() != framesize)
  {
    throw std::runtime_error(
      $("Invalid source frame size {0} != {1}!",
        source->framesize(),
        framesize));
  }

  if (sink->framesize() != framesize)
  {
    throw std::runtime_error(
      $("Invalid sink frame size {0} != {1}!",
        sink->framesize(),
        framesize));
  }

  buffers.input.resize(framesize * 2);
  buffers.output.resize(framesize * 2);

  std::vector<float> window(framesize);

  std::iota(window.begin(), window.end(), 0.0f);

  std::transform(window.begin(), window.end(), window.begin(),
    [&](float value) { return 0.5f - 0.5f * std::cos(PI2 * value / window.size()); });

  windows.analysis = window;
  windows.synthesis = window;

  const float unitygain = hopsize / std::inner_product(
    window.begin(), window.end(), window.begin(), 0.0f);

  std::transform(windows.synthesis.begin(), windows.synthesis.end(), windows.synthesis.begin(),
    [&](float value) { return value * unitygain; });
}

void StftPipeline::operator()(const size_t index, const std::vector<float>& input, std::vector<float>& output)
{
  for (size_t i = 0; i < framesize; ++i)
  {
    buffers.input[i] = buffers.input[i + framesize];
    buffers.input[i + framesize] = input[i];

    output[i] = buffers.output[i];

    buffers.output[i] = buffers.output[i + framesize];
    buffers.output[i + framesize] = 0;
  }

  for (size_t hop = 0; (hop + framesize) < (framesize * 2); hop += hopsize)
  {
    reject(hop, buffers.input.data(), frame, windows.analysis);
    fft(frame, dft);

    (*this)(dft);

    ifft(dft, frame);
    inject(hop, buffers.output.data(), frame, windows.synthesis);
  }
}

void StftPipeline::reject(const size_t hop, const float* input, std::vector<float>& frame, const std::vector<float>& window)
{
  for (size_t i = 0; i < frame.size(); ++i)
  {
    frame[i] = input[hop + i] * window[i];
  }
}

void StftPipeline::inject(const size_t hop, float* const output, const std::vector<float>& frame, const std::vector<float>& window)
{
  for (size_t i = 0; i < frame.size(); ++i)
  {
    output[hop + i] += frame[i] * window[i];
  }
}

void StftPipeline::fft(const std::vector<float>& frame, std::vector<std::complex<float>>& dft)
{
  pocketfft::r2c(
    { frame.size() },
    { sizeof(float) },
    { sizeof(std::complex<float>) },
    0,
    true,
    frame.data(),
    dft.data(),
    1.0f / frame.size());
}

void StftPipeline::ifft(const std::vector<std::complex<float>>& dft, std::vector<float>& frame)
{
  pocketfft::c2r(
    { frame.size() },
    { sizeof(std::complex<float>) },
    { sizeof(float) },
    0,
    false,
    dft.data(),
    frame.data(),
    1.0f);
}
