#pragma once

#include <voyx/Header.h>
#include <voyx/etc/FIFO.h>
#include <voyx/io/Source.h>

#include <RtAudio.h>

class AudioSource : public Source<voyx_t>
{

public:

  AudioSource(const std::string& name, voyx_t samplerate, size_t framesize, size_t buffersize);

  void open() override;
  void close() override;

  void start() override;
  void stop() override;

  bool read(const size_t index, std::function<void(const voyx::vector<voyx_t> frame)> callback) override;

private:

  struct InputFrame
  {
    size_t index;
    std::vector<voyx_t> frame;
  };

  const std::string audio_device_name;
  FIFO<InputFrame> audio_frame_buffer;

  RtAudio audio;

  static int callback(void* output_frame_data, void* input_frame_data, uint32_t framesize, double timestamp, RtAudioStreamStatus status, void* $this);
  static void error(RtAudioError::Type type, const std::string& error);

};
