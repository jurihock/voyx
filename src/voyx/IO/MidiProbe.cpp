#include <voyx/IO/MidiProbe.h>

#include <voyx/Source.h>

#include <rtmidi/RtMidi.h>

std::string MidiProbe::operator()()
{
  const std::vector<std::string> columns =
  {
    "#",
    "Midi Device Name"
  };

  std::map<std::string, std::vector<std::string>> table
  {
    { "#", {} },
    { "Midi Device Name", {} }
  };

  // POPULATE TABLE

  std::shared_ptr<RtMidiIn> midi;
  size_t ports;

  try
  {
    midi = std::make_shared<RtMidiIn>();
    ports = midi->getPortCount();
  }
  catch (const RtMidiError& error)
  {
    ports = 0;
  }

  for (size_t index = 0; index < ports; ++index)
  {
    std::string name;

    try
    {
      name = midi->getPortName(index);
    }
    catch (const RtMidiError& error)
    {
      continue;
    }

    const size_t offset = table[columns.front()].size() + 1;

    const size_t rows = 1;

    for (const auto& [column, _] : table)
    {
      table[column].resize(offset + rows);
    }

    table["#"][offset - 1] = "-";
    table["#"][offset] = std::to_string(index);
    table["Midi Device Name"][offset] = name;
  }

  // STRINGIFY TABLE

  const size_t rows = table[columns.front()].size();

  std::ostringstream stream;

  if (!rows)
  {
    stream << "Sadly no midi devices detected! :~(" << std::endl;

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
