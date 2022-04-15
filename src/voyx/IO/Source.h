#pragma once

#include <chrono>
#include <cmath>
#include <functional>
#include <string>
#include <vector>

template<typename T>
class Source
{

public:

  Source(size_t samplerate, size_t framesize, size_t buffersize) :
    source_samplerate(samplerate),
    source_framesize(framesize),
    source_buffersize(buffersize),
    source_timeout(std::chrono::milliseconds(
      static_cast<std::chrono::milliseconds::rep>(
        std::ceil(1e3 * framesize / samplerate)))) {}

  virtual ~Source() {}

  size_t samplerate() const { return source_samplerate; }
  size_t framesize() const { return source_framesize; }
  size_t buffersize() const { return source_buffersize; }
  const std::chrono::milliseconds& timeout() const { return source_timeout; }

  virtual void open() {};
  virtual void close() {};

  virtual void start() {};
  virtual void stop() {};

  virtual bool read(const size_t index, std::function<void(const std::vector<T>& frame)> callback) = 0;

private:

  const size_t source_samplerate;
  const size_t source_framesize;
  const size_t source_buffersize;
  const std::chrono::milliseconds source_timeout;

};
