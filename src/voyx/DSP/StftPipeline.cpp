#include <voyx/DSP/StftPipeline.h>

#include <voyx/Source.h>

StftPipeline::StftPipeline(const size_t samplerate, const size_t framesize, const size_t hopsize, std::shared_ptr<Source<float>> source, std::shared_ptr<Sink<float>> sink) :
  Pipeline(source, sink),
  samplerate(samplerate),
  framesize(framesize),
  hopsize(hopsize),
  fft(framesize)
{
  if (source->samplerate() != samplerate)
  {
    throw std::runtime_error(
      $("Incompatible source sample rate {0} != {1}!",
        source->samplerate(),
        samplerate));
  }

  if (sink->samplerate() != samplerate)
  {
    throw std::runtime_error(
      $("Incompatible sink sample rate {0} != {1}!",
        sink->samplerate(),
        samplerate));
  }

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

  data.frames.resize(hops.size() * fft.tdsize());
  data.dfts.resize(hops.size() * fft.fdsize());

  data.views.frames.resize(hops.size());
  data.views.dfts.resize(hops.size());

  for (size_t i = 0; i < hops.size(); ++i)
  {
    data.views.frames[i] = std::span<float>(
      data.frames.data() + i * fft.tdsize(),
      fft.tdsize());

    data.views.dfts[i] = std::span<std::complex<float>>(
      data.dfts.data() + i * fft.fdsize(),
      fft.fdsize());
  }

  data.input.resize(2 * framesize);
  data.output.resize(2 * framesize);

  const std::vector<float> window = Window<float>(framesize);

  windows.analysis = window;
  windows.synthesis = window;

  const float unitygain = hopsize / std::inner_product(
    windows.synthesis.begin(), windows.synthesis.end(), windows.synthesis.begin(), 0.0f);

  std::transform(windows.synthesis.begin(), windows.synthesis.end(), windows.synthesis.begin(),
    [unitygain](float value) { return value * unitygain; });
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
    const size_t j = i + framesize;

    data.input[i] = data.input[j];
    data.input[j] = input[i];

    data.output[i] = data.output[j];
    data.output[j] = 0;
  }

  if (parallelize)
  {
    for (size_t i = 0; i < hops.size(); ++i)
    {
      reject(hops[i], data.input, data.views.frames[i], windows.analysis);
    }

    #pragma omp parallel for
    for (size_t i = 0; i < hops.size(); ++i)
    {
      fft.fft(data.views.frames[i], data.views.dfts[i]);
    }

    (*this)(index, data.input, data.views.dfts);

    #pragma omp parallel for
    for (size_t i = 0; i < hops.size(); ++i)
    {
      fft.ifft(data.views.dfts[i], data.views.frames[i]);
    }

    for (size_t i = 0; i < hops.size(); ++i)
    {
      inject(hops[i], data.output, data.views.frames[i], windows.synthesis);
    }
  }
  else
  {
    for (size_t i = 0; i < hops.size(); ++i)
    {
      reject(hops[i], data.input, data.views.frames[i], windows.analysis);
    }

    for (size_t i = 0; i < hops.size(); ++i)
    {
      fft.fft(data.views.frames[i], data.views.dfts[i]);
    }

    (*this)(index, data.input, data.views.dfts);

    for (size_t i = 0; i < hops.size(); ++i)
    {
      fft.ifft(data.views.dfts[i], data.views.frames[i]);
    }

    for (size_t i = 0; i < hops.size(); ++i)
    {
      inject(hops[i], data.output, data.views.frames[i], windows.synthesis);
    }
  }

  for (size_t i = 0; i < framesize; ++i)
  {
    output[i] = data.output[i];
  }
}

void StftPipeline::reject(const size_t hop, const std::vector<float>& input, std::span<float>& frame, const std::vector<float>& window)
{
  for (size_t i = 0; i < frame.size(); ++i)
  {
    frame[i] = input[hop + i] * window[i];
  }
}

void StftPipeline::inject(const size_t hop, std::vector<float>& output, const std::span<float>& frame, const std::vector<float>& window)
{
  for (size_t i = 0; i < frame.size(); ++i)
  {
    output[hop + i] += frame[i] * window[i];
  }
}
