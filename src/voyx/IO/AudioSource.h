#pragma once

#include <voyx/IO/FIFO.h>
#include <voyx/IO/Source.h>

#include <rtaudio/RtAudio.h>

class AudioSource : public Source<float>
{

public:

  AudioSource(const std::string& name, size_t samplerate, size_t framesize, size_t buffersize);

  void open() override;
  void close() override;

  void start() override;
  void stop() override;

  bool read(const size_t index, std::function<void(const std::vector<float>& frame)> callback) override;

private:

  struct InputFrame
  {
    size_t index;
    std::vector<float> frame;
    double timestamp;
    RtAudioStreamStatus status;
  };

  const std::string audio_device_name;
  FIFO<InputFrame> audio_frame_buffer;
  RtAudio audio;

  static int callback(void* output_frame_data, void* input_frame_data, uint32_t framesize, double timestamp, RtAudioStreamStatus status, void* $this);

};
