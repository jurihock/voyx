#include <voyx/etc/WAV.h>

#include <voyx/Source.h>
#include <voyx/alg/SRC.h>

#include <dr_wav.h>

void WAV::read(const std::string& path, std::vector<double>& data, const voyx_t samplerate)
{
  std::vector<float> nativedata;

  WAV::read(path, nativedata, samplerate);

  data.assign(nativedata.begin(), nativedata.end());
}

void WAV::read(const std::string& path, std::vector<float>& data, const voyx_t samplerate)
{
  drwav wav;

  if (drwav_init_file(&wav, path.c_str(), nullptr) != DRWAV_TRUE)
  {
    throw std::runtime_error(
      $("Unable to open \"{0}\"!", path));
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
      data[i] = data[i * channels];

      for (size_t j = 1; j < channels; ++j)
      {
        data[i] += data[i * channels + j];
      }

      data[i] /= channels;
    }

    data.resize(samples);
  }

  if (wav.sampleRate != samplerate)
  {
    SRC<float> convert({ wav.sampleRate, samplerate });

    std::vector<float> buffer(data.size() * convert.quotient());

    convert(data, buffer);

    data.assign(buffer.begin(), buffer.end());
  }
}

void WAV::write(const std::string& path, const std::vector<double>& data, const voyx_t samplerate)
{
  std::vector<float> nativedata(data.begin(), data.end());

  WAV::write(path, nativedata, samplerate);
}

void WAV::write(const std::string& path, const std::vector<float>& data, const voyx_t samplerate)
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
  format.channels = channels;
  format.sampleRate = static_cast<size_t>(samplerate);

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
