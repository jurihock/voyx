#include <voyx/IO/MidiObserver.h>

#include <voyx/Source.h>

MidiObserver::MidiObserver(const std::string& name) :
  midi_device_name(name)
{
  midi.setErrorCallback(&MidiObserver::error, this);
}

MidiObserver::~MidiObserver()
{
  stop();
}

void MidiObserver::start()
{
  stop();

  const uint32_t ports = midi.getPortCount();

  if (!ports)
  {
    throw std::runtime_error(
      "No midi sources available!");
  }

  std::optional<uint32_t> id;

  for (uint32_t i = 0; i < ports; ++i)
  {
    const std::string name = midi.getPortName(i);

    if (midi_device_name.empty())
    {
      id = i;
      break;
    }

    if (!$$::imatch(name, ".*" + midi_device_name + ".*"))
    {
      continue;
    }

    id = i;
    break;
  }

  if (!id)
  {
    throw std::runtime_error(
      $("Midi source \"{0}\" not found!",
        midi_device_name));
  }

  midi.setCallback(&MidiObserver::callback, this);
  midi.openPort(id.value(), "Voyx Input");
}

void MidiObserver::stop()
{
  if (midi.isPortOpen())
  {
    midi.cancelCallback();
    midi.closePort();
  }
}

void MidiObserver::callback(double timestamp, std::vector<unsigned char>* message, void* $this)
{
  const std::vector<uint8_t> bytes((*message).begin(), (*message).end());

  std::ostringstream bits;

  for (uint8_t byte : bytes)
  {
    bits << std::bitset<8>(byte) << " ";
  }

  LOG(INFO) << "MIDI: " << bits.str();
}

void MidiObserver::error(RtMidiError::Type type, const std::string& error, void* $this)
{
  LOG(ERROR) << "Midi source error: " << error;
}
