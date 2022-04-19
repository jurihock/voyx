#pragma once

#include <voyx/Header.h>

#include <rtmidi/RtMidi.h>

class MidiObserver
{

public:

  MidiObserver(const std::string& name);
  ~MidiObserver();

  void start();
  void stop();

private:

  const std::string midi_device_name;

  RtMidiIn midi;

  static void callback(double timestamp, std::vector<unsigned char>* message, void* $this);
  static void error(RtMidiError::Type type, const std::string& error, void* $this);

};
