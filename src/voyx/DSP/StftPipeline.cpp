#include <voyx/DSP/StftPipeline.h>

#include <voyx/Source.h>

StftPipeline::StftPipeline(const voyx_t samplerate, const size_t framesize, const size_t hopsize, std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink) :
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
    data.views.frames[i] = std::span<voyx_t>(
      data.frames.data() + i * fft.tdsize(),
      fft.tdsize());

    data.views.dfts[i] = std::span<std::complex<voyx_t>>(
      data.dfts.data() + i * fft.fdsize(),
      fft.fdsize());
  }

  data.input.resize(2 * framesize);
  data.output.resize(2 * framesize);

  const std::vector<voyx_t> window = Window<voyx_t>(framesize);

  windows.analysis = window;
  windows.synthesis = window;

  const voyx_t unitygain = hopsize / std::inner_product(
    windows.synthesis.begin(), windows.synthesis.end(), windows.synthesis.begin(), 0.0f);

  std::transform(windows.synthesis.begin(), windows.synthesis.end(), windows.synthesis.begin(),
    [unitygain](voyx_t value) { return value * unitygain; });
}

void StftPipeline::warmup()
{
  if (parallelize)
  {
    const size_t frames = 60 * sink->samplerate() / sink->framesize();

    LOG(INFO) << "Begin warmup (" << frames << " frames)";

    Timer<std::chrono::seconds> timer;

    std::vector<voyx_t> input(framesize);
    std::vector<voyx_t> output(framesize);

    timer.tic();
    for (size_t index = 0; index < frames; ++index)
    {
      (*this)(index, input, output);
    }
    timer.toc();

    LOG(INFO) << "Finish warmup (" << timer.str() << ")";
  }
}

void StftPipeline::operator()(const size_t index, const std::vector<voyx_t>& input, std::vector<voyx_t>& output)
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
    reject(hops, data.input, data.views.frames, windows.analysis);

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

    inject(hops, data.output, data.views.frames, windows.synthesis);
  }
  else
  {
    reject(hops, data.input, data.views.frames, windows.analysis);
    fft.fft(data.views.frames, data.views.dfts);
    (*this)(index, data.input, data.views.dfts);
    fft.ifft(data.views.dfts, data.views.frames);
    inject(hops, data.output, data.views.frames, windows.synthesis);
  }

  for (size_t i = 0; i < framesize; ++i)
  {
    output[i] = data.output[i];
  }
}

void StftPipeline::reject(const std::vector<size_t>& hops, const std::vector<voyx_t>& input, const std::matrix<voyx_t>& frames, const std::vector<voyx_t>& window)
{
  for (size_t i = 0; i < hops.size(); ++i)
  {
    for (size_t j = 0; j < window.size(); ++j)
    {
      frames[i][j] = input[hops[i] + j] * window[j];
    }
  }
}

void StftPipeline::inject(const std::vector<size_t>& hops, std::vector<voyx_t>& output, const std::matrix<voyx_t>& frames, const std::vector<voyx_t>& window)
{
  for (size_t i = 0; i < hops.size(); ++i)
  {
    for (size_t j = 0; j < window.size(); ++j)
    {
      output[hops[i] + j] += frames[i][j] * window[j];
    }
  }
}
