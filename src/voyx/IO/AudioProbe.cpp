#include <voyx/IO/AudioProbe.h>

#include <voyx/Source.h>

#include <rtaudio/RtAudio.h>

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

  RtAudio audio;

  const size_t devices = audio.getDeviceCount();

  for (size_t index = 0; index < devices; ++index)
  {
    const RtAudio::DeviceInfo device = audio.getDeviceInfo(index);

    if (!device.probed)
    {
      continue;
    }

    const std::string name = device.name;
    const std::string inputs = std::to_string(device.inputChannels) + (device.isDefaultInput ? " *" : "");
    const std::string outputs = std::to_string(device.outputChannels) + (device.isDefaultOutput ? " *" : "");
    const std::string duplex = std::to_string(device.duplexChannels);
    std::vector<std::string> samplerates(device.sampleRates.size());
    std::vector<std::string> formats;

    for (size_t i = 0; i < samplerates.size(); ++i)
    {
      samplerates[i] = std::to_string(device.sampleRates[i]);

      if (device.sampleRates[i] == device.preferredSampleRate)
      {
        samplerates[i] += " *";
      }
    }

    for (const auto& format : well_known_formats)
    {
      if (format.second & device.nativeFormats)
      {
        formats.push_back(format.first);
      }
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

  const size_t rows = table[columns.front()].size();
  const size_t spacer = 3;

  std::map<std::string, size_t> column_widths;
  size_t total_column_width = 0;

  std::ostringstream stream;

  for (const auto& column : columns)
  {
    size_t column_width = column.size();

    for (const auto& row : table[column])
    {
      column_width = std::max(column_width, row.size());
    }

    column_widths[column] = column_width + spacer;
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
