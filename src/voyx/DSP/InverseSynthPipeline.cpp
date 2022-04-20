#include <voyx/DSP/InverseSynthPipeline.h>

#include <voyx/Source.h>

#include <mlinterp/mlinterp.hpp>

InverseSynthPipeline::InverseSynthPipeline(const voyx_t samplerate, const size_t framesize, const size_t hopsize,
                                           std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink,
                                           std::shared_ptr<MidiObserver> midi, std::shared_ptr<Plot> plot) :
  StftPipeline(samplerate, framesize, hopsize, source, sink),
  vocoder(samplerate, framesize, hopsize),
  midi(midi),
  plot(plot)
{
  if (midi != nullptr)
  {
    const voyx_t concertpitch = midi->concertpitch();

    dftbins.resize(framesize / 2 + 1);
    {
      std::iota(dftbins.begin(), dftbins.end(), 0);
    }

    dftfreqs.resize(dftbins.size());
    {
      std::transform(dftbins.begin(), dftbins.end(), dftfreqs.begin(),
        [samplerate, framesize](voyx_t i) { return i * samplerate / framesize; });
    }

    dftkeys.resize(dftfreqs.size());
    {
      std::transform(dftfreqs.begin(), dftfreqs.end(), dftkeys.begin(),
        [concertpitch](voyx_t i) { return (i > 0) ? 69 + 12 * std::log2(i / concertpitch) : 0; });
    }

    midikeys = midi->keys();
    midifreqs = midi->freqs();

    midibins.resize(128);
    {
      std::vector<voyx_t> x0(dftkeys);
      std::vector<voyx_t> x1(midikeys);

      std::vector<voyx_t> y0(dftbins);
      std::vector<voyx_t> y1(midibins.size());

      const size_t n0[] = { x0.size() };
      const size_t n1 = x1.size();

      mlinterp::interp(
        n0,        n1,
        y0.data(), y1.data(),
        x0.data(), x1.data());

      midibins = y1;
    }

    dftfreqs.resize(framesize / 2 + 1);
    {
      std::vector<voyx_t> x0(midibins);
      std::vector<voyx_t> x1(dftbins);

      std::vector<voyx_t> y0(midifreqs);
      std::vector<voyx_t> y1(dftfreqs.size());

      const size_t n0[] = { x0.size() };
      const size_t n1 = x1.size();

      mlinterp::interp(
        n0,        n1,
        y0.data(), y1.data(),
        x0.data(), x1.data());

      dftfreqs = y1;
    }
  }

  if (plot != nullptr)
  {
    plot->xrange(samplerate / 2);
    plot->xlim(0, 2.5e3);
    plot->ylim(0, 1);
  }
}

void InverseSynthPipeline::operator()(const size_t index, const std::vector<voyx_t>& signal,
                                      const std::vector<std::span<std::complex<voyx_t>>>& dfts)
{
  if (midi != nullptr)
  {
    for (auto dft : dfts)
    {
      std::vector<voyx_t> midimask(midi->mask());
      std::vector<voyx_t> dftmask(dftbins.size());
      {
        std::vector<voyx_t> x0(midibins);
        std::vector<voyx_t> x1(dftbins);

        std::vector<voyx_t> y0(midimask);
        std::vector<voyx_t> y1(dftmask.size());

        const size_t n0[] = { x0.size() };
        const size_t n1 = x1.size();

        mlinterp::interp(
          n0,        n1,
          y0.data(), y1.data(),
          x0.data(), x1.data());

        dftmask = y1;
      }

      if (plot != nullptr)
      {
        plot->plot(dftmask);
      }

      for (size_t i = 0; i < dft.size(); ++i)
      {
        dft[i].real(dftmask[i]);
        dft[i].imag(dftfreqs[i]);
      }
    }
  }
  else
  {
    vocoder.encode(dfts);
  }

  vocoder.decode(dfts);
}
