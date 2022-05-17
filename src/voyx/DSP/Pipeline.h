#pragma once

#include <voyx/Header.h>

#include <voyx/IO/Source.h>
#include <voyx/IO/Sink.h>

template<typename T = voyx_t>
class Pipeline
{

public:

  Pipeline(std::shared_ptr<Source<T>> source, std::shared_ptr<Sink<T>> sink) :
    source(source),
    sink(sink)
  {
  }

  virtual ~Pipeline() {}

  void open()
  {
    close();

    source->open();
    sink->open();
  }

  void close()
  {
    stop();

    source->close();
    sink->close();
  }

  void start()
  {
    start(0);
  }

  void start(const std::chrono::duration<double> duration)
  {
    const double seconds = static_cast<double>(
      std::chrono::duration_cast<std::chrono::seconds>(duration).count());

    const size_t frames = static_cast<size_t>(
      std::ceil(seconds * sink->samplerate() / sink->framesize()));

    start(frames);
  }

  void start(const size_t frames)
  {
    stop();

    source->start();
    sink->start();

    onstart(frames);

    if (frames > 0)
    {
      stop();
    }
  }

  void stop()
  {
    onstop();

    source->stop();
    sink->stop();
  }

public:

  const std::shared_ptr<Source<T>> source;
  const std::shared_ptr<Sink<T>> sink;

  virtual void onstart(const size_t frames) = 0;
  virtual void onstop() = 0;

};
