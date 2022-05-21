#include <voyx/io/AudioProbe.h>

#include <voyx/Source.h>

#include <RtAudio.h>

std::string AudioProbe::operator()()
{
  const std::map<std::string, RtAudioFormat> well_known_formats
  {
    { "8-bit int", RTAUDIO_SINT8 },
    { "16-bit int", RTAUDIO_SINT16 },
    { "24-bit int", RTAUDIO_SINT24 },
    { "32-bit int", RTAUDIO_SINT32 },
    { "32-bit float", RTAUDIO_FLOAT32 },
    { "64-bit float", RTAUDIO_FLOAT64 },
  };

  const std::vector<std::string> columns =
  {
    "#",
    "Audio Device Name",
    "Inputs",
    "Outputs",
    "Duplex",
    "Sample Rates",
    "Data Formats"
  };

  std::map<std::string, std::vector<std::string>> table
  {
    { "#", {} },
    { "Audio Device Name", {} },
    { "Inputs", {} },
    { "Outputs", {} },
    { "Duplex", {} },
    { "Sample Rates", {} },
    { "Data Formats", {} },
  };

  // POPULATE TABLE

  std::shared_ptr<RtAudio> audio;
  size_t devices;

  try
  {
    audio = std::make_shared<RtAudio>();
    devices = audio->getDeviceCount();
  }
  catch (const RtAudioError& error)
  {
    LOG(ERROR) << error.what();
    
    devices = 0;
  }

  for (size_t index = 0; index < devices; ++index)
  {
    std::string name;
    std::string inputs;
    std::string outputs;
    std::string duplex;
    std::vector<std::string> samplerates;
    std::vector<std::string> formats;

    try
    {
      const RtAudio::DeviceInfo device = audio->getDeviceInfo(index);

      if (!device.probed)
      {
        continue;
      }

      name = device.name;
      inputs = std::to_string(device.inputChannels) + (device.isDefaultInput ? " *" : "");
      outputs = std::to_string(device.outputChannels) + (device.isDefaultOutput ? " *" : "");
      duplex = std::to_string(device.duplexChannels);

      for (const auto samplerate : device.sampleRates)
      {
        samplerates.push_back(std::to_string(samplerate) +
          (samplerate == device.preferredSampleRate ? " *" : ""));
      }

      for (const auto format : well_known_formats)
      {
        if (format.second & device.nativeFormats)
        {
          formats.push_back(format.first);
        }
      }
    }
    catch (const RtAudioError& error)
    {
      LOG(ERROR) << error.what();
      
      continue;
    }

    const size_t offset = table[columns.front()].size() + 1;

    const size_t rows = std::max(
    {
      samplerates.size(), formats.size(), size_t(1)
    });

    for (const auto& [column, _] : table)
    {
      table[column].resize(offset + rows);
    }

    table["#"][offset - 1] = "-";
    table["#"][offset] = std::to_string(index);
    table["Audio Device Name"][offset] = name;
    table["Inputs"][offset] = inputs;
    table["Outputs"][offset] = outputs;
    table["Duplex"][offset] = duplex;

    for (size_t i = 0; i < samplerates.size(); ++i)
    {
      table["Sample Rates"][offset + i] = samplerates[i];
    }

    for (size_t i = 0; i < formats.size(); ++i)
    {
      table["Data Formats"][offset + i] = formats[i];
    }
  }

  // STRINGIFY TABLE

  const size_t rows = table[columns.front()].size();

  std::ostringstream stream;

  if (!rows)
  {
    stream << "Sadly no audio devices detected! :~(" << std::endl;

    return stream.str();
  }

  std::map<std::string, size_t> column_widths;
  size_t total_column_width = 0;

  for (const auto& column : columns)
  {
    size_t column_width = column.size();

    for (const auto& row : table[column])
    {
      column_width = std::max(column_width, row.size());
    }

    column_widths[column] = column_width + 3;
    total_column_width += column_widths[column];
  }

  for (const auto& column : columns)
  {
    stream << std::setw(column_widths[column]) << std::left << column;

    if (column == columns.back())
    {
      stream << std::endl;
    }
  }

  for (size_t i = 0; i < rows; ++i)
  {
    if (table[columns.front()][i] == "-")
    {
      stream << std::string(total_column_width, '-');
    }
    else
    {
      for (const auto& column : columns)
      {
        stream << std::setw(column_widths[column]) << std::left << table[column][i];
      }
    }
    stream << std::endl;
  }

  return stream.str();
}
