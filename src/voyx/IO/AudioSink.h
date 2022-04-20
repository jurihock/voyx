#pragma once

#include <voyx/Header.h>
#include <voyx/ETC/FIFO.h>
#include <voyx/IO/Sink.h>

#include <rtaudio/RtAudio.h>

class AudioSink : public Sink<voyx_t>
{

public:

  AudioSink(const std::string& name, voyx_t samplerate, size_t framesize, size_t buffersize);

  void open() override;
  void close() override;

  void start() override;
  void stop() override;

  bool write(const size_t index, const std::vector<voyx_t>& frame) override;

private:

  struct OutputFrame
  {
    size_t index;
    std::vector<voyx_t> frame;
  };

  const std::string audio_device_name;
  FIFO<OutputFrame> audio_frame_buffer;

  RtAudio audio;

  static int callback(void* output_frame_data, void* input_frame_data, uint32_t framesize, double timestamp, RtAudioStreamStatus status, void* $this);
  static void error(RtAudioError::Type type, const std::string& error);

};
