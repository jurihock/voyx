#include <voyx/DSP/InverseSynthPipeline.h>

#include <voyx/Source.h>

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

    midibins = $$::interp<voyx_t>(midikeys, dftkeys, dftbins);
    dftfreqs = $$::interp<voyx_t>(dftbins, midibins, midifreqs);
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
      const auto midimask = midi->mask();
      const auto dftmask = $$::interp<voyx_t>(dftbins, midibins, midimask);

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
