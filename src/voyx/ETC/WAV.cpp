#include <voyx/ETC/WAV.h>
#include <voyx/Voyx.h>

#define DR_WAV_IMPLEMENTATION
#include <dr_libs/dr_wav.h>

void WAV::read(const std::string& path, std::vector<float>& data, const size_t samplerate)
{
  drwav wav;

  if (drwav_init_file(&wav, path.c_str(), nullptr) != DRWAV_TRUE)
  {
    throw std::runtime_error(
      $("Unable to open \"{0}\"!", path));
  }

  if (wav.sampleRate != samplerate)
  {
    drwav_uninit(&wav);

    throw std::runtime_error(
      $("Invalid file sample rate {0} != {1}!", wav.sampleRate, samplerate));
  }

  const size_t samples = wav.totalPCMFrameCount;
  const size_t channels = wav.channels;
  const size_t bytes = samples * channels * sizeof(float);

  if (bytes > DRWAV_SIZE_MAX)
  {
    drwav_uninit(&wav);

    throw std::runtime_error(
      $("The file is too large \"{0}\"!", path));
  }

  data.resize(samples * channels);

  if (drwav_read_pcm_frames_f32(&wav, samples, data.data()) != samples)
  {
    drwav_uninit(&wav);

    throw std::runtime_error(
      $("Unable to read \"{0}\"!", path));
  }

  drwav_uninit(&wav);

  if (channels > 1)
  {
    for (size_t i = 0; i < samples; ++i)
    {
      for (size_t j = 1; j < channels; ++j)
      {
        data[i] += data[i * channels + j];
      }

      data[i] /= channels;
    }

    data.resize(samples);
  }
}

void WAV::write(const std::string& path, const std::vector<float>& data, const size_t samplerate)
{
  const size_t samples = data.size();
  const size_t channels = 1;
  const size_t bytes = samples * channels * sizeof(float);

  if (bytes > DRWAV_SIZE_MAX)
  {
    throw std::runtime_error(
      $("The file is too large \"{0}\"!", path));
  }

  drwav wav;
  drwav_data_format format;

  format.container = drwav_container_riff;
  format.format = DR_WAVE_FORMAT_IEEE_FLOAT;
  format.bitsPerSample = sizeof(float) * 8;
  format.channels = 1;
  format.sampleRate = samplerate;

  if (drwav_init_file_write(&wav, path.c_str(), &format, nullptr) != DRWAV_TRUE)
  {
    throw std::runtime_error(
      $("Unable to create \"{0}\"!", path));
  }

  if (drwav_write_pcm_frames(&wav, data.size(), data.data()) != data.size())
  {
    throw std::runtime_error(
      $("Unable to write \"{0}\"!", path));
  }

  if (drwav_uninit(&wav) != DRWAV_SUCCESS)
  {
    throw std::runtime_error(
      $("Unable to finish \"{0}\"!", path));
  }
}
