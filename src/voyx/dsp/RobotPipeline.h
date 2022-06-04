#pragma once

#include <voyx/Header.h>
#include <voyx/alg/Oscillator.h>
#include <voyx/dsp/SdftPipeline.h>
#include <voyx/io/MidiObserver.h>
#include <voyx/ui/Plot.h>

class RobotPipeline : public SdftPipeline
{

public:

  RobotPipeline(const voyx_t samplerate, const size_t framesize, const size_t dftsize,
                std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink,
                std::shared_ptr<MidiObserver> midi, std::shared_ptr<Plot> plot);

  void operator()(const size_t index,
                  voyx::matrix<std::complex<voyx_t>> dfts) override;

private:

  std::shared_ptr<MidiObserver> midi;
  std::shared_ptr<Plot> plot;

  std::vector<Oscillator<voyx_t>> osc;
  voyx_t f0;

};
