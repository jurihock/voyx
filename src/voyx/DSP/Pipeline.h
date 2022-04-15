#pragma once

#include <voyx/IO/Source.h>
#include <voyx/IO/Sink.h>

#include <voyx/IO/AudioSource.h>
#include <voyx/IO/AudioSink.h>

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
    doloop = false;

    if (thread != nullptr)
    {
      if (thread->joinable())
      {
        thread->join();
      }

      thread = nullptr;
    }

    source->stop();
    sink->stop();
  }

  virtual void operator()(const size_t index, const std::vector<T>& input, std::vector<T>& output) = 0;

private:

  std::shared_ptr<Source<T>> source;
  std::shared_ptr<Sink<T>> sink;

  std::shared_ptr<std::thread> thread;

  bool doloop;

  void loop(const size_t frames)
  {
    std::vector<T> output(sink->framesize());

    size_t index = 0;

    if (frames > 0)
    {
      while (doloop && index < frames)
      {
        const bool ok = source->read(index, [&](const std::vector<T>& input)
        {
          (*this)(index, input, output);
        });

        if (ok)
        {
          sink->write(index, output);
        }

        index += ok ? 1 : 0;
      }
    }
    else
    {
      while(doloop)
      {
        const bool ok = source->read(index, [&](const std::vector<T>& input)
        {
          (*this)(index, input, output);
        });

        if (ok)
        {
          const bool sync = (index & 1)
            && (std::dynamic_pointer_cast<AudioSource>(source) == nullptr)
            && (std::dynamic_pointer_cast<AudioSink>(sink) != nullptr);

          if (sync)
          {
            std::this_thread::sleep_for(sink->timeout());
          }

          sink->write(index, output);
        }

        index += ok ? 1 : 0;
      }
    }
  }

};
