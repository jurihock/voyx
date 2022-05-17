#include <voyx/DSP/StftPipeline.h>

#include <voyx/Source.h>

StftPipeline::StftPipeline(const voyx_t samplerate, const size_t framesize, const size_t hopsize, std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink) :
  SyncPipeline<voyx_t>(source, sink),
  samplerate(samplerate),
  framesize(framesize),
  hopsize(hopsize),
  fft(framesize)
{
  for (size_t hop = 0; (hop + framesize) < (framesize * 2); hop += hopsize)
  {
    hops.push_back(hop);
  }

  data.frames.resize(hops.size() * fft.tdsize());
  data.dfts.resize(hops.size() * fft.fdsize());

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

void StftPipeline::operator()(const size_t index, const voyx::vector<voyx_t> input, voyx::vector<voyx_t> output)
{
  for (size_t i = 0; i < framesize; ++i)
  {
    const size_t j = i + framesize;

    data.input[i] = data.input[j];
    data.input[j] = input[i];

    data.output[i] = data.output[j];
    data.output[j] = 0;
  }

  voyx::matrix<voyx_t> frames(data.frames, fft.tdsize());
  voyx::matrix<std::complex<voyx_t>> dfts(data.dfts, fft.fdsize());

  reject(hops, data.input, frames, windows.analysis);
  fft.fft(frames, dfts);

  (*this)(index, data.input, dfts);

  fft.ifft(dfts, frames);
  inject(hops, data.output, frames, windows.synthesis);

  for (size_t i = 0; i < framesize; ++i)
  {
    output[i] = data.output[i];
  }
}

void StftPipeline::reject(const std::vector<size_t>& hops, const voyx::vector<voyx_t> input, voyx::matrix<voyx_t> frames, const std::vector<voyx_t>& window)
{
  for (size_t i = 0; i < hops.size(); ++i)
  {
    for (size_t j = 0; j < window.size(); ++j)
    {
      frames[i][j] = input[hops[i] + j] * window[j];
    }
  }
}

void StftPipeline::inject(const std::vector<size_t>& hops, voyx::vector<voyx_t> output, const voyx::matrix<voyx_t> frames, const std::vector<voyx_t>& window)
{
  for (size_t i = 0; i < hops.size(); ++i)
  {
    for (size_t j = 0; j < window.size(); ++j)
    {
      output[hops[i] + j] += frames[i][j] * window[j];
    }
  }
}
