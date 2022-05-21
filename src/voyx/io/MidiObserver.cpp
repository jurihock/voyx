#include <voyx/io/MidiObserver.h>

#include <voyx/Source.h>

MidiObserver::MidiObserver(const std::string& name, const voyx_t concertpitch) :
  midi_device_name(name),
  midi_concert_pitch(concertpitch),
  midi_key_state(128)
{
  midi.setErrorCallback(&MidiObserver::error, this);

  start();
}

MidiObserver::~MidiObserver()
{
  stop();
}

voyx_t MidiObserver::concertpitch() const
{
  return midi_concert_pitch;
}

std::vector<int> MidiObserver::state()
{
  std::lock_guard lock(mutex);
  return midi_key_state;
}

std::vector<voyx_t> MidiObserver::mask()
{
  const std::vector<int> src = state();
  std::vector<voyx_t> dst(src.size());

  std::transform(src.begin(), src.end(), dst.begin(),
    [](voyx_t value) { return value / voyx_t(127); });

  return dst;
}

std::vector<voyx_t> MidiObserver::imask()
{
  const std::vector<int> src = state();
  std::vector<voyx_t> dst(src.size());

  std::transform(src.begin(), src.end(), dst.begin(),
    [](voyx_t value) { return (127 - value) / voyx_t(127); });

  return dst;
}

void MidiObserver::start()
{
  stop();

  if (midi_device_name.empty())
  {
    throw std::runtime_error(
      "No midi source name specified!");
  }

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

void MidiObserver::dump(std::vector<unsigned char>* message)
{
  const std::vector<uint8_t> bytes(
    (*message).begin(), (*message).end());

  std::ostringstream bits;

  for (uint8_t byte : bytes)
  {
    bits << std::bitset<8>(byte) << " ";
  }

  LOG(INFO) << "MIDI: " << bits.str();
}

void MidiObserver::callback(double timestamp, std::vector<unsigned char>* message, void* $this)
{
  // https://www.midi.org/specifications-old/item/table-1-summary-of-midi-message
  // https://www.midi.org/specifications-old/item/table-2-expanded-messages-list-status-bytes

  // dump(message);

  if ((*message).empty())
  {
    return;
  }

  const bool reset = ((*message)[0] == 0xFF);

  if (reset)
  {
    auto observer = static_cast<MidiObserver*>($this);

    std::lock_guard lock(observer->mutex);

    std::fill(
      observer->midi_key_state.begin(),
      observer->midi_key_state.end(),
      0);

    // LOG(INFO) << "MIDI: reset";

    return;
  }

  if ((*message).size() < 3)
  {
    return;
  }

  const uint8_t status = ((*message)[0] >> 4);

  const bool on = (status == 0b1001) || (status == 0b1010);
  const bool off = (status == 0b1000);

  const int key = (*message)[1] & 0b0111'1111;
  const int velocity = (*message)[2] & 0b0111'1111;

  if (on || off)
  {
    auto observer = static_cast<MidiObserver*>($this);

    std::lock_guard lock(observer->mutex);

    observer->midi_key_state[key] = on ? velocity : 0;

    // LOG(INFO) << $("MIDI: {0} key={1:03d} velocity={2:03d}", on ? "on " : "off", key, velocity);
  }
}

void MidiObserver::error(RtMidiError::Type type, const std::string& error, void* $this)
{
  LOG(ERROR) << "Midi source error: " << error;
}
