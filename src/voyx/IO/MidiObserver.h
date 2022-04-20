#pragma once

#include <voyx/Header.h>

#include <rtmidi/RtMidi.h>

class MidiObserver
{

public:

  MidiObserver(const std::string& name);
  ~MidiObserver();

  std::vector<int> keys();
  std::vector<voyx_t> mask();
  std::vector<voyx_t> imask();
  std::vector<voyx_t> bins(const size_t framesize, const size_t samplerate, const voyx_t concertpitch);

  void start();
  void stop();

private:

  const std::string midi_device_name;
  std::vector<int> midi_key_state;

  RtMidiIn midi;

  std::mutex mutex;

  static void dump(std::vector<unsigned char>* message);
  static void callback(double timestamp, std::vector<unsigned char>* message, void* $this);
  static void error(RtMidiError::Type type, const std::string& error, void* $this);

};
