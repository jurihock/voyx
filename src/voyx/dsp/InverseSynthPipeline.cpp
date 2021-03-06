#include <voyx/dsp/InverseSynthPipeline.h>

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

    dftfreqs = $$::dft::freqs<voyx_t>(samplerate, framesize);
    dftbins = $$::dft::bins<voyx_t>(framesize);
    dftkeys = $$::midi::keys(dftfreqs, concertpitch);
    midifreqs = $$::midi::freqs<voyx_t>(concertpitch);
    midikeys = $$::midi::keys<voyx_t>();
    midibins = $$::interp(midikeys, dftkeys, dftbins);
    dftfreqs = $$::interp(dftbins, midibins, midifreqs);
  }

  if (plot != nullptr)
  {
    plot->xrange(samplerate / 2);
    plot->xlim(0, 2.5e3);
    plot->ylim(0, 1);
  }
}

void InverseSynthPipeline::operator()(const size_t index,
                                      const voyx::vector<voyx_t> signal,
                                      voyx::matrix<std::complex<voyx_t>> dfts)
{
  if (midi != nullptr)
  {
    for (auto dft : dfts)
    {
      const auto midimask = midi->mask();
      const auto dftmask = $$::interp(dftbins, midibins, midimask);

      for (size_t i = 0; i < dft.size(); ++i)
      {
        dft[i].real(dftmask[i]);
        dft[i].imag(dftfreqs[i]);
      }

      if (plot != nullptr)
      {
        plot->plot(dftmask);
      }
    }
  }
  else
  {
    vocoder.encode(dfts);
  }

  vocoder.decode(dfts);
}
