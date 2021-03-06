#pragma once

#include <voyx/Header.h>
#include <voyx/alg/Vocoder.h>
#include <voyx/dsp/SdftPipeline.h>
#include <voyx/io/MidiObserver.h>
#include <voyx/ui/Plot.h>

class SdftTestPipeline : public SdftPipeline<double>
{

public:

  SdftTestPipeline(const voyx_t samplerate, const size_t framesize, const size_t dftsize,
                   std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink,
                   std::shared_ptr<MidiObserver> midi, std::shared_ptr<Plot> plot);

  void operator()(const size_t index,
                  voyx::matrix<std::complex<double>> dfts) override;

private:

  Vocoder<double> vocoder;

  std::shared_ptr<MidiObserver> midi;
  std::shared_ptr<Plot> plot;

};
