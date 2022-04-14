#pragma once

#include <voyx/IO/FIFO.h>
#include <voyx/IO/Sink.h>

#include <rtaudio/RtAudio.h>

class AudioSink : public Sink<float>
{

public:

  AudioSink(const std::string& name, size_t samplerate, size_t framesize, size_t buffersize);

  void open() override;
  void close() override;

  void start() override;
  void stop() override;

  void write(const std::vector<float>& frame) override;

private:

  struct OutputFrame
  {
    size_t index;
    std::vector<float> frame;
  };

  const std::string audio_device_name;
  FIFO<OutputFrame> audio_frame_buffer;
  RtAudio audio;

  static int callback(void* output_frame_data, void* input_frame_data, uint32_t framesize, double timestamp, RtAudioStreamStatus status, void* $this);

};
