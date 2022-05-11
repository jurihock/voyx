#pragma once

#include <voyx/Header.h>
#include <voyx/DSP/StftPipeline.h>
#include <voyx/IO/MidiObserver.h>
#include <voyx/UI/Plot.h>

class TestPipeline : public StftPipeline
{

public:

  TestPipeline(const voyx_t samplerate, const size_t framesize, const size_t hopsize,
               std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink,
               std::shared_ptr<MidiObserver> midi, std::shared_ptr<Plot> plot);

  void operator()(const size_t index, const std::vector<voyx_t>& signal,
                  const std::matrix<std::complex<voyx_t>>& dfts) override;

private:

  std::shared_ptr<MidiObserver> midi;
  std::shared_ptr<Plot> plot;

};
