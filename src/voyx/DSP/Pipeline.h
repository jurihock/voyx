#pragma once

#include <voyx/IO/Source.h>
#include <voyx/IO/Sink.h>

#include <chrono>
#include <memory>
#include <thread>

template<typename T>
class Pipeline
{

public:

  Pipeline(std::shared_ptr<Source<T>> source, std::shared_ptr<Sink<T>> sink) :
    source(source),
    sink(sink)
  {
  }

  ~Pipeline()
  {
    close();
  }

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

    doloop = true;

    thread = std::make_shared<std::thread>(
      [frames, this](){ loop(frames); });

    if (frames > 0)
    {
      if (thread->joinable())
      {
        thread->join();
      }

      stop();
    }
  }

  void stop()
  {
    source->stop();
    sink->stop();

    doloop = false;

    if (thread != nullptr)
    {
      if (thread->joinable())
      {
        thread->join();
      }

      thread = nullptr;
    }
  }

  virtual void operator()(const std::vector<T>& input, std::vector<T>& output) = 0;

private:

  std::shared_ptr<Source<T>> source;
  std::shared_ptr<Sink<T>> sink;

  std::shared_ptr<std::thread> thread;

  bool doloop;

  void loop(const size_t frames)
  {
    std::vector<T> output(sink->framesize());

    if (frames > 0)
    {
      for (size_t frame = 0; frame < frames && doloop;)
      {
        const bool ok = source->read([&](const std::vector<T>& input)
        {
          (*this)(input, output);
        });

        if (ok)
        {
          sink->write(output);
        }

        frame += ok ? 1 : 0;
      }
    }
    else
    {
      while(doloop)
      {
        const bool ok = source->read([&](const std::vector<T>& input)
        {
          (*this)(input, output);
        });

        if (ok)
        {
          sink->write(output);
        }
      }
    }
  }

};
