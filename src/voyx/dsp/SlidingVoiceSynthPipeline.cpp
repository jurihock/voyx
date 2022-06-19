#include <voyx/dsp/SlidingVoiceSynthPipeline.h>

#include <voyx/Source.h>

SlidingVoiceSynthPipeline::SlidingVoiceSynthPipeline(const voyx_t samplerate, const size_t framesize, const size_t dftsize,
                                                     std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink,
                                                     std::shared_ptr<MidiObserver> midi, std::shared_ptr<Plot> plot) :
  SdftPipeline(samplerate, framesize, dftsize, source, sink),
  midi(midi),
  plot(plot),
  vocoder(samplerate, dftsize * 2, 1),
  lifter(1e-3, samplerate, dftsize * 2),
  pda({ 50, 1000 }, samplerate),
  ptr(442)
{
  if (plot != nullptr)
  {
    plot->xrange(samplerate / 2);
    plot->xlim(0, 2e3);
    plot->ylim(-120, 0);
  }
}

void SlidingVoiceSynthPipeline::operator()(const size_t index,
                                           voyx::matrix<std::complex<double>> dfts)
{
  std::set<voyx_t> frequencies;
  bool sustain = false;

  if (midi != nullptr)
  {
    const auto values = midi->frequencies();
    frequencies.insert(values.begin(), values.end());

    sustain = midi->sustain();
  }

  if (sustain)
  {
    frequencies.merge(this->frequencies);
  }

  this->frequencies = frequencies;

  vocoder.encode(dfts);

  std::vector<double> envelope(dfts.stride());
  std::vector<double> spectrum(dfts.stride());
  std::vector<double> cepstrum(dfts.stride() * 2);

  std::vector<double> abs0(dfts.stride());
  std::vector<double> abs1(dfts.stride());

  lifter.lowpass<$$::real>(dfts.front(), envelope, spectrum, cepstrum);

  auto f0 = ptr(pda(spectrum));

  for (auto dft : dfts)
  {
    lifter.divide<$$::real>(dft, envelope);

    for (size_t i = 0; i < dft.size(); ++i)
    {
      abs0[i] = dft[i].real();

      dft[i].real(0);
    }

    for (const auto f1 : frequencies)
    {
      const auto ratio = f1 / f0;
      const auto invratio = 1 / ratio;

      $$::interp(abs0, abs1, ratio);

      for (size_t i = 0; i < dft.size(); ++i)
      {
        dft[i].real(std::max(abs1[i] * invratio, dft[i].real()));
      }
    }

    lifter.multiply<$$::real>(dft, envelope);
  }

  vocoder.decode(dfts);

  if (plot != nullptr)
  {
    for (size_t i = 0; i < spectrum.size(); ++i)
    {
      spectrum[i] *= 20;
    }

    plot->plot(spectrum);
  }
}
