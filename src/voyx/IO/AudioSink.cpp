#include <voyx/IO/AudioSink.h>

#include <voyx/Voyx.h>

#include <optional>

AudioSink::AudioSink(const std::string& name, size_t samplerate, size_t framesize, size_t buffersize) :
  Sink(samplerate, framesize, buffersize),
  audio_device_name(name),
  audio_frame_buffer(
    buffersize,
    [framesize](size_t index)
    {
      auto output = new OutputFrame();
      output->index = index;
      output->frame.resize(framesize);
      return output;
    },
    [](OutputFrame* output)
    {
      delete output;
    })
{
}

void AudioSink::open()
{
  if (audio.isStreamRunning())
  {
    audio.stopStream();
  }

  if (audio.isStreamOpen())
  {
    audio.closeStream();
  }

  std::optional<uint32_t> id;

  if (audio_device_name.empty())
  {
    id = audio.getDefaultOutputDevice();
  }
  else
  {
    const uint32_t devices = audio.getDeviceCount();

    for (uint32_t i = 0; i < devices; ++i)
    {
      const RtAudio::DeviceInfo device = audio.getDeviceInfo(i);

      if (!device.probed)
      {
        continue;
      }

      if (device.outputChannels < 1)
      {
        continue;
      }

      if (!$$::imatch(device.name, "*" + audio_device_name + "*"))
      {
        continue;
      }

      id = i;
      break;
    }
  }

  if (!id)
  {
    throw std::runtime_error(
      $("Audio sink \"{0}\" not found!",
        audio_device_name));
  }

  RtAudio::StreamParameters stream_parameters;
  stream_parameters.deviceId = id.value();
  stream_parameters.nChannels = 1;
  stream_parameters.firstChannel = 0;

  const RtAudioFormat stream_format = RTAUDIO_FLOAT32;
  const uint32_t stream_samplerate = static_cast<uint32_t>(samplerate());
  uint32_t stream_framesize = static_cast<uint32_t>(framesize());

  audio.openStream(
    &stream_parameters,
    nullptr,
    stream_format,
    stream_samplerate,
    &stream_framesize,
    &AudioSink::callback,
    this);

  if (stream_framesize != framesize())
  {
    throw std::runtime_error(
      $("Unexpected audio sink stream frame size {0} != {1}!",
        stream_framesize,
        framesize()));
  }
}

void AudioSink::close()
{
  if (audio.isStreamRunning())
  {
    audio.stopStream();
  }

  if (audio.isStreamOpen())
  {
    audio.closeStream();
  }
}

void AudioSink::start()
{
  if (!audio.isStreamOpen())
  {
    return;
  }

  if (audio.isStreamRunning())
  {
    audio.stopStream();
  }

  audio.startStream();
}

void AudioSink::stop()
{
  if (!audio.isStreamOpen())
  {
    return;
  }

  if (!audio.isStreamRunning())
  {
    return;
  }

  audio.stopStream();
}

void AudioSink::write(const std::vector<float>& frame)
{
  const auto ok = audio_frame_buffer.write([&](OutputFrame& output)
  {
    output.frame = frame;
  });

  if (!ok)
  {
    std::cout << "UNABLE TO WRITE TO OUTPUT FRAME FIFO!" << std::endl;
  }
}

int AudioSink::callback(void* output_frame_data, void* input_frame_data, uint32_t framesize, double timestamp, RtAudioStreamStatus status, void* $this)
{
  auto& audio_frame_buffer = static_cast<AudioSink*>($this)->audio_frame_buffer;

  const auto ok = audio_frame_buffer.read([&](OutputFrame& output)
  {
    if (output.frame.size() != framesize)
    {
      std::cout << "UNEXPECTED OUTPUT FRAME SIZE!" << std::endl;
    }

    const size_t bytes = std::min(
      output.frame.size(),
      static_cast<size_t>(framesize) *
      sizeof(output.frame.front()));

    std::memcpy(output_frame_data, output.frame.data(), bytes);
  });

  if (!ok)
  {
    std::cout << "UNABLE TO READ FROM OUTPUT FRAME FIFO!" << std::endl;
  }

  if (status != 0)
  {
    std::cout << "AUDIO SINK STREAM STATUS " << status << std::endl;
  }

  return 0;
}
