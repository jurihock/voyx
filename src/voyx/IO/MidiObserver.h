#pragma once

#include <voyx/Header.h>

#include <RtMidi.h>

class MidiObserver
{

public:

  MidiObserver(const std::string& name, const voyx_t concertpitch);
  ~MidiObserver();

  voyx_t concertpitch() const;

  std::vector<int> state();

  std::vector<voyx_t> mask();
  std::vector<voyx_t> imask();

  void start();
  void stop();

private:

  const std::string midi_device_name;
  const voyx_t midi_concert_pitch;

  std::vector<int> midi_key_state;

  RtMidiIn midi;

  std::mutex mutex;

  static void dump(std::vector<unsigned char>* message);
  static void callback(double timestamp, std::vector<unsigned char>* message, void* $this);
  static void error(RtMidiError::Type type, const std::string& error, void* $this);

};
