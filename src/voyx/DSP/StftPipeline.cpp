#include <voyx/DSP/StftPipeline.h>

#include <voyx/Source.h>

#include <pocketfft/pocketfft_hdronly.h>

StftPipeline::StftPipeline(size_t framesize, size_t hopsize, std::shared_ptr<Source<float>> source, std::shared_ptr<Sink<float>> sink) :
  Pipeline(source, sink),
  framesize(framesize),
  hopsize(hopsize)
{
  if (source->framesize() != framesize)
  {
    throw std::runtime_error(
      $("Incompatible source frame size {0} != {1}!",
        source->framesize(),
        framesize));
  }

  if (sink->framesize() != framesize)
  {
    throw std::runtime_error(
      $("Incompatible sink frame size {0} != {1}!",
        sink->framesize(),
        framesize));
  }

  for (size_t hop = 0; (hop + framesize) < (framesize * 2); hop += hopsize)
  {
    hops.push_back(hop);
  }

  data.frames.resize(hops.size());
  data.dfts.resize(hops.size());

  for (size_t i = 0; i < hops.size(); ++i)
  {
    data.frames[i].resize(framesize);
    data.dfts[i].resize(framesize / 2 + 1);
  }

  data.input.resize(framesize * 2);
  data.output.resize(framesize * 2);

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

void StftPipeline::warmup()
{
  if (parallelize)
  {
    const size_t frames = 60 * sink->samplerate() / sink->framesize();

    LOG(INFO) << "Begin warmup (" << frames << " frames)";

    Timer<std::chrono::seconds> timer;

    std::vector<float> input(framesize);
    std::vector<float> output(framesize);

    timer.tic();
    for (size_t index = 0; index < frames; ++index)
    {
      (*this)(index, input, output);
    }
    timer.toc();

    LOG(INFO) << "Finish warmup (" << timer.str() << ")";
  }
}

void StftPipeline::operator()(const size_t index, const std::vector<float>& input, std::vector<float>& output)
{
  for (size_t i = 0; i < framesize; ++i)
  {
    data.input[i] = data.input[i + framesize];
    data.input[i + framesize] = input[i];

    data.output[i] = data.output[i + framesize];
    data.output[i + framesize] = 0;
  }

  if (parallelize)
  {
    const float* input = data.input.data();
    float* const output = data.output.data();

    for (size_t i = 0; i < hops.size(); ++i)
    {
      const size_t hop = hops[i];

      std::vector<float>& frame = data.frames[i];

      reject(hop, input, frame, windows.analysis);
    }

    #pragma omp parallel for
    for (size_t i = 0; i < hops.size(); ++i)
    {
      std::vector<float>& frame = data.frames[i];
      std::vector<std::complex<float>>& dft = data.dfts[i];

      fft(frame, dft);
    }

    for (size_t i = 0; i < hops.size(); ++i)
    {
      std::vector<std::complex<float>>& dft = data.dfts[i];

      (*this)(dft);
    }

    #pragma omp parallel for
    for (size_t i = 0; i < hops.size(); ++i)
    {
      std::vector<float>& frame = data.frames[i];
      std::vector<std::complex<float>>& dft = data.dfts[i];

      ifft(dft, frame);
    }

    for (size_t i = 0; i < hops.size(); ++i)
    {
      const size_t hop = hops[i];

      std::vector<float>& frame = data.frames[i];

      inject(hop, output, frame, windows.synthesis);
    }
  }
  else
  {
    const float* input = data.input.data();
    float* const output = data.output.data();

    std::vector<float>& frame = data.frames.front();
    std::vector<std::complex<float>>& dft = data.dfts.front();

    for (size_t i = 0; i < hops.size(); ++i)
    {
      const size_t hop = hops[i];

      reject(hop, input, frame, windows.analysis);
      fft(frame, dft);

      (*this)(dft);

      ifft(dft, frame);
      inject(hop, output, frame, windows.synthesis);
    }
  }

  for (size_t i = 0; i < framesize; ++i)
  {
    output[i] = data.output[i];
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
