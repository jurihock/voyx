#pragma once

#include <voyx/Header.h>

template<typename T = voyx_t>
class Sink
{

public:

  Sink(voyx_t samplerate, size_t framesize, size_t buffersize) :
    sink_samplerate(samplerate),
    sink_framesize(framesize),
    sink_buffersize(buffersize),
    sink_timeout(std::chrono::milliseconds(
      static_cast<std::chrono::milliseconds::rep>(
        std::ceil(1e3 * framesize / samplerate)))) {}

  virtual ~Sink() {}

  voyx_t samplerate() const { return sink_samplerate; }
  size_t framesize() const { return sink_framesize; }
  size_t buffersize() const { return sink_buffersize; }
  const std::chrono::milliseconds& timeout() const { return sink_timeout; }

  virtual void open() {};
  virtual void close() {};

  virtual void start() {};
  virtual void stop() {};

  virtual bool write(const size_t index, const voyx::vector<T> frame) = 0;
  virtual bool sync() { return true; }

private:

  const voyx_t sink_samplerate;
  const size_t sink_framesize;
  const size_t sink_buffersize;
  const std::chrono::milliseconds sink_timeout;

};
