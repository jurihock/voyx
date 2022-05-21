#pragma once

#include <voyx/Header.h>
#include <voyx/DSP/SdftPipeline.h>
#include <voyx/IO/MidiObserver.h>
#include <voyx/UI/Plot.h>

class SdftTestPipeline : public SdftPipeline
{

public:

  SdftTestPipeline(const voyx_t samplerate, const size_t framesize, const size_t dftsize,
                   std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink,
                   std::shared_ptr<MidiObserver> midi, std::shared_ptr<Plot> plot);

  void operator()(const size_t index,
                  voyx::matrix<std::complex<voyx_t>> dfts) override;

private:

  std::shared_ptr<MidiObserver> midi;
  std::shared_ptr<Plot> plot;

};
