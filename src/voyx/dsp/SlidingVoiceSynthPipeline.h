#pragma once

#include <voyx/Header.h>
#include <voyx/alg/Lifter.h>
#include <voyx/alg/NaivePitchTracking.h>
#include <voyx/alg/SpectralPitchDetector.h>
#include <voyx/alg/Vocoder.h>
#include <voyx/dsp/SdftPipeline.h>
#include <voyx/io/MidiObserver.h>
#include <voyx/ui/Plot.h>

class SlidingVoiceSynthPipeline : public SdftPipeline<double>
{

public:

  SlidingVoiceSynthPipeline(const voyx_t samplerate, const size_t framesize, const size_t dftsize,
                            std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink,
                            std::shared_ptr<MidiObserver> midi, std::shared_ptr<Plot> plot);

  void operator()(const size_t index,
                  voyx::matrix<std::complex<double>> dfts) override;

private:

  std::shared_ptr<MidiObserver> midi;
  std::shared_ptr<Plot> plot;

  Vocoder<double> vocoder;
  Lifter<double> lifter;

  SpectralPitchDetector<double> pda;
  NaivePitchTracking ptr;

  std::set<voyx_t> frequencies;

};
