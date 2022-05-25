#pragma once

#include <voyx/Header.h>
#include <voyx/etc/Logger.h>
#include <voyx/etc/Timer.h>
#include <voyx/dsp/Pipeline.h>

template<typename T = voyx_t>
class SyncPipeline : public Pipeline<T>
{

public:

  SyncPipeline(std::shared_ptr<Source<T>> source, std::shared_ptr<Sink<T>> sink) :
    Pipeline<T>(source, sink)
  {
  }

protected:

  void onstart(const size_t frames) override
  {
    doloop = true;

    thread = std::make_shared<std::thread>(
      [frames, this](){ loop(frames); });

    if (frames > 0)
    {
      if (thread->joinable())
      {
        thread->join();
      }
    }
  }

  void onstop() override
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
  }

  virtual void operator()(const size_t index, const voyx::vector<T> input, voyx::vector<T> output) = 0;

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

    std::vector<T> output(this->sink->framesize());

    size_t index = 0;
    bool ok = true;

    timers.outer.tic();

    if (frames > 0)
    {
      while (doloop && index < frames)
      {
        ok = this->source->read(index, [&](const voyx::vector<T> input)
        {
          timers.outer.toc();
          timers.outer.tic();

          timers.inner.tic();
          (*this)(index, input, output);
          timers.inner.toc();
        });

        if (ok)
        {
          this->sink->sync();
          this->sink->write(index, output);
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

        ok = this->source->read(index, [&](const voyx::vector<T> input)
        {
          timers.outer.toc();
          timers.outer.tic();

          timers.inner.tic();
          (*this)(index, input, output);
          timers.inner.toc();
        });

        if (ok)
        {
          this->sink->sync();
          this->sink->write(index, output);
        }

        index += ok ? 1 : 0;
      }
    }
  }

};
