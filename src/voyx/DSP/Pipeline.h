#pragma once

#include <voyx/Header.h>

#include <voyx/IO/Source.h>
#include <voyx/IO/Sink.h>

#include <voyx/IO/AudioSource.h>
#include <voyx/IO/AudioSink.h>

#include <voyx/ETC/Logger.h>
#include <voyx/ETC/Timer.h>

template<typename T = voyx_t>
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
    warmup();

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

protected:

  std::shared_ptr<Source<T>> source;
  std::shared_ptr<Sink<T>> sink;

  virtual void warmup() {}

private:

  std::shared_ptr<std::thread> thread;
  bool doloop = false;

  void loop(const size_t frames)
  {
    struct
    {
      Timer<std::chrono::milliseconds> inner;
      Timer<std::chrono::milliseconds> outer;
    }
    timers;

    std::vector<T> output(sink->framesize());

    size_t index = 0;

    timers.outer.tic();

    if (frames > 0)
    {
      while (doloop && index < frames)
      {
        const bool ok = source->read(index, [&](const std::vector<T>& input)
        {
          timers.outer.toc();
          timers.outer.tic();

          timers.inner.tic();
          (*this)(index, input, output);
          timers.inner.toc();
        });

        if (ok)
        {
          sink->write(index, output);
        }

        index += ok ? 1 : 0;
      }

      LOG(INFO)
        << "Timing: \t"
        << "inner " << timers.inner.str() << "\t"
        << "outer " << timers.outer.str();

      timers.inner.cls();
      timers.outer.cls();
    }
    else
    {
      const bool sync =
        (std::dynamic_pointer_cast<AudioSource>(source) == nullptr) &&
        (std::dynamic_pointer_cast<AudioSink>(sink) != nullptr);

      auto millis = [](const std::chrono::steady_clock::duration& duration)
      {
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
      };

      auto now = []()
      {
        return std::chrono::steady_clock::now();
      };

      auto timestamp = now();

      while(doloop)
      {
        if (millis(now() - timestamp) > 5000)
        {
          LOG(INFO)
            << "Timing: \t"
            << "inner " << timers.inner.str() << "\t"
            << "outer " << timers.outer.str();

          timers.inner.cls();
          timers.outer.cls();

          timestamp = now();
        }

        const bool ok = source->read(index, [&](const std::vector<T>& input)
        {
          timers.outer.toc();
          timers.outer.tic();

          timers.inner.tic();
          (*this)(index, input, output);
          timers.inner.toc();
        });

        if (ok)
        {
          if (sync && (index & 1))
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
