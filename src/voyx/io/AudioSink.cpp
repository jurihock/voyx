#include <voyx/io/AudioSink.h>

#include <voyx/Source.h>

AudioSink::AudioSink(const std::string& name, voyx_t samplerate, size_t framesize, size_t buffersize) :
  Sink(samplerate, framesize, buffersize),
  audio_device_name(name),
  audio_sync_semaphore(buffersize),
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

  const uint32_t devices = audio.getDeviceCount();

  if (!devices)
  {
    throw std::runtime_error(
      "No audio sinks available!");
  }

  std::optional<uint32_t> id;

  if (audio_device_name.empty())
  {
    id = audio.getDefaultOutputDevice();
  }
  else
  {
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

      if (!$$::imatch(device.name, ".*" + audio_device_name + ".*"))
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

  const RtAudio::DeviceInfo device = audio.getDeviceInfo(id.value());

  RtAudio::StreamParameters stream_parameters;
  stream_parameters.deviceId = id.value();
  stream_parameters.nChannels = 1;
  stream_parameters.firstChannel = 0;

  const RtAudioFormat stream_format = (typeid(voyx_t) == typeid(float)) ? RTAUDIO_FLOAT32 : RTAUDIO_FLOAT64;
  uint32_t stream_samplerate = device.preferredSampleRate;
  uint32_t stream_framesize = static_cast<uint32_t>(framesize());

  for (const uint32_t native_samplerate : device.sampleRates)
  {
    if (native_samplerate == samplerate())
    {
      stream_samplerate = native_samplerate;
      break;
    }
  }

  audio_samplerate_converter = { samplerate(), stream_samplerate };

  stream_framesize *= audio_samplerate_converter.quotient();

  audio.openStream(
    &stream_parameters,
    nullptr,
    stream_format,
    stream_samplerate,
    &stream_framesize,
    &AudioSink::callback,
    this,
    nullptr,
    &AudioSink::error);

  if (stream_framesize != framesize() * audio_samplerate_converter.quotient())
  {
    throw std::runtime_error(
      $("Unexpected audio sink stream frame size {0} != {1} * {2}!",
        stream_framesize, framesize(), audio_samplerate_converter.quotient()));
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

bool AudioSink::write(const size_t index, const voyx::vector<voyx_t> frame)
{
  const bool ok = audio_frame_buffer.write([&](OutputFrame& output)
  {
    output.frame.assign(frame.begin(), frame.end());
  });

  if (!ok)
  {
    LOG(WARNING) << $("Audio sink fifo overflow!");
  }

  return ok;
}

bool AudioSink::sync()
{
  return audio_sync_semaphore.try_acquire_for(timeout());
}

int AudioSink::callback(void* output_frame_data, void* input_frame_data, uint32_t framesize, double timestamp, RtAudioStreamStatus status, void* $this)
{
  auto& audio_frame_buffer = static_cast<AudioSink*>($this)->audio_frame_buffer;
  auto& audio_samplerate_converter = static_cast<AudioSink*>($this)->audio_samplerate_converter;
  auto& audio_sync_semaphore = static_cast<AudioSink*>($this)->audio_sync_semaphore;

  const auto ok = audio_frame_buffer.read([&](OutputFrame& output)
  {
    if (framesize != output.frame.size() * audio_samplerate_converter.quotient())
    {
      LOG(WARNING) << $("Unexpected output frame size {0} != {1} * {2}!",
                        framesize, output.frame.size(), audio_samplerate_converter.quotient());
    }

    voyx::vector<voyx_t> src = { output.frame.data(), output.frame.size() };
    voyx::vector<voyx_t> dst = { static_cast<voyx_t*>(output_frame_data), framesize };

    audio_samplerate_converter(src, dst);
  });

  if (!ok)
  {
    LOG(WARNING) << $("Audio sink fifo underflow!");
  }

  if (status == RTAUDIO_OUTPUT_UNDERFLOW)
  {
    LOG(WARNING) << $("Audio sink stream underflow!");
  }
  else if (status)
  {
    LOG(WARNING) << $("Audio sink stream status {0}!", status);
  }

  audio_sync_semaphore.release();

  return 0;
}

void AudioSink::error(RtAudioError::Type type, const std::string& error)
{
  LOG(ERROR) << "Audio sink stream error: " << error;
}
