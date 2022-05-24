#pragma once

#include <voyx/Header.h>
#include <voyx/alg/Vocoder.h>
#include <voyx/dsp/StftPipeline.h>
#include <voyx/io/MidiObserver.h>
#include <voyx/ui/Plot.h>

class StftTestPipeline : public StftPipeline
{

public:

  StftTestPipeline(const voyx_t samplerate, const size_t framesize, const size_t hopsize,
                   std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink,
                   std::shared_ptr<MidiObserver> midi, std::shared_ptr<Plot> plot);

  void operator()(const size_t index,
                  const voyx::vector<voyx_t> signal,
                  voyx::matrix<std::complex<voyx_t>> dfts) override;

private:

  Vocoder<voyx_t> vocoder;

  std::shared_ptr<MidiObserver> midi;
  std::shared_ptr<Plot> plot;

};
