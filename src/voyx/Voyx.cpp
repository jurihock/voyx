#include <voyx/Voyx.h>
#include <voyx/IO/Probe.h>

// TEST
#include <voyx/DSP/BypassPipeline.h>
#include <voyx/IO/AudioSource.h>
#include <voyx/IO/AudioSink.h>
#include <voyx/IO/FileSource.h>
#include <voyx/IO/FileSink.h>
#include <voyx/IO/NullSource.h>
#include <voyx/IO/NullSink.h>
#include <voyx/IO/SineSource.h>

#include <cxxopts/cxxopts.hpp>

#include <condition_variable>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <mutex>

INITIALIZE_EASYLOGGINGPP

const int OK = EXIT_SUCCESS;
const int NOK = EXIT_FAILURE;

std::condition_variable shutdown;
std::mutex mutex;

void onsignal(int value)
{
  shutdown.notify_one();
}

int main(int argc, char** argv)
{
  std::signal(SIGINT, onsignal);

  cxxopts::Options options("voyx");

  options.add_options()
    ("t,test",   "Run test routine")
    ("h,help",   "Print this help")
    ("l,list",   "List available audio devices")
    ("i,input",  "Input audio device or .wav file")
    ("o,output", "Output audio device or .wav file");

  const auto args = options.parse(argc, argv);

  if (argc < 2)
  {
    std::cout << options.help() << std::endl;

    return OK;
  }

  if (args.count("help"))
  {
    std::cout << options.help() << std::endl;

    return OK;
  }

  if (args.count("list"))
  {
    Probe probe;

    std::cout << probe();

    return OK;
  }

  if (args.count("test"))
  {
    const size_t samplerate = 44100;
    const size_t framesize = 1024;
    const size_t buffersize = 1000;

    // auto source = std::make_shared<NullSource>(samplerate, framesize, buffersize);
    // auto source = std::make_shared<SineSource>(0.5, 250, samplerate, framesize, buffersize);
    auto source = std::make_shared<FileSource>("angela.wav", samplerate, framesize, buffersize);
    // auto source = std::make_shared<AudioSource>("microphone", samplerate, framesize, buffersize);

    // auto sink = std::make_shared<NullSink>(samplerate, framesize, buffersize);
    // auto sink = std::make_shared<FileSink>("test.wav", samplerate, framesize, buffersize);
    auto sink = std::make_shared<AudioSink>("speakers", samplerate, framesize, buffersize);

    auto pipe = std::make_shared<BypassPipeline>(source, sink);

    pipe->open();

    // pipe->start(std::chrono::seconds(3));
    pipe->start();

    std::unique_lock lock(mutex);
    shutdown.wait(lock);

    pipe->close();

    return OK;
  }

  std::unique_lock lock(mutex);
  shutdown.wait(lock);

  return OK;
}
