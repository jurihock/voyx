#pragma once

#include <voyx/Header.h>

template<typename T = voyx_t>
class Source
{

public:

  Source(voyx_t samplerate, size_t framesize, size_t buffersize) :
    source_samplerate(samplerate),
    source_framesize(framesize),
    source_buffersize(buffersize),
    source_timeout(std::chrono::milliseconds(
      static_cast<std::chrono::milliseconds::rep>(
        std::ceil(1e3 * framesize / samplerate)))) {}

  virtual ~Source() {}

  voyx_t samplerate() const { return source_samplerate; }
  size_t framesize() const { return source_framesize; }
  size_t buffersize() const { return source_buffersize; }
  const std::chrono::milliseconds& timeout() const { return source_timeout; }

  virtual void open() {};
  virtual void close() {};

  virtual void start() {};
  virtual void stop() {};

  virtual bool read(const size_t index, std::function<void(const voyx::vector<T> frame)> callback) = 0;

private:

  const voyx_t source_samplerate;
  const size_t source_framesize;
  const size_t source_buffersize;
  const std::chrono::milliseconds source_timeout;

};
