#pragma once

#include <voyx/Header.h>
#include <voyx/DSP/StftPipeline.h>
#include <voyx/ETC/Vocoder.h>
#include <voyx/IO/MidiObserver.h>
#include <voyx/UI/Plot.h>

class InverseSynthPipeline : public StftPipeline
{

public:

  InverseSynthPipeline(const voyx_t samplerate, const size_t framesize, const size_t hopsize,
                       std::shared_ptr<Source<voyx_t>> source, std::shared_ptr<Sink<voyx_t>> sink,
                       std::shared_ptr<MidiObserver> midi, std::shared_ptr<Plot> plot);

  void operator()(const size_t index, const std::vector<voyx_t>& signal,
                  const std::vector<std::span<std::complex<voyx_t>>>& dfts) override;

private:

  Vocoder<voyx_t> vocoder;

  std::shared_ptr<MidiObserver> midi;
  std::shared_ptr<Plot> plot;

  std::vector<voyx_t> dftbins, dftfreqs, dftkeys;
  std::vector<voyx_t> midibins, midifreqs, midikeys;

};
