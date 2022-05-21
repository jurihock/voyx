#pragma once

#include <voyx/Header.h>
#include <voyx/ETC/Logger.h>
#include <voyx/ETC/Timer.h>
#include <voyx/DSP/Pipeline.h>

template<typename T = voyx_t>
class AsyncPipeline : public Pipeline<T>
{

public:

  AsyncPipeline(std::shared_ptr<Source<T>> source, std::shared_ptr<Sink<T>> sink) :
    Pipeline<T>(source, sink)
  {
  }

protected:

  void onstart(const size_t frames) override
  {
    timers.read.cls();
    timers.write.cls();

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

  virtual void begin(const size_t index, const voyx::vector<T> input) = 0;

  void end(const size_t index, const voyx::vector<T> output)
  {
    timers.write.toc();
    timers.write.tic();

    this->sink->sync();
    this->sink->write(index, output);
  }

private:

  struct
  {
    Timer<std::chrono::milliseconds> read;
    Timer<std::chrono::milliseconds> write;
  }
  timers;

  std::shared_ptr<std::thread> thread;
  bool doloop = false;

  void loop(const size_t frames)
  {
    size_t index = 0;
    bool ok = true;

    timers.read.tic();
    timers.write.tic();

    if (frames > 0)
    {
      while (doloop && index < frames)
      {
        ok = this->source->read(index, [&](const voyx::vector<T> input)
        {
          timers.read.toc();
          timers.read.tic();

          begin(index, input);
        });

        index += ok ? 1 : 0;
      }

      LOG(INFO)
        << "Timing: \t"
        << "read  " << timers.read.str() << "\t"
        << "write " << timers.write.str();
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
            << "read  " << timers.read.str() << "\t"
            << "write " << timers.write.str();

          timers.read.cls();
          timers.write.cls();

          timestamp = now();
        }

        ok = this->source->read(index, [&](const voyx::vector<T> input)
        {
          timers.read.toc();
          timers.read.tic();

          begin(index, input);
        });

        index += ok ? 1 : 0;
      }
    }
  }

};
