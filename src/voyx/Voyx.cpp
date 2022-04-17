// TODO object factory
#include <voyx/DSP/TestPipeline.h>
#include <voyx/DSP/BypassPipeline.h>
#include <voyx/DSP/StftPipeline.h>
#include <voyx/IO/AudioSource.h>
#include <voyx/IO/AudioSink.h>
#include <voyx/IO/FileSource.h>
#include <voyx/IO/FileSink.h>
#include <voyx/IO/NullSource.h>
#include <voyx/IO/NullSink.h>
#include <voyx/IO/SineSource.h>

#include <voyx/Source.h>
#include <voyx/IO/AudioProbe.h>
#include <voyx/UI/Plot.h>
#include <voyx/UI/QPlot.h>

#include <cxxopts/cxxopts.hpp>

INITIALIZE_EASYLOGGINGPP

const int OK = EXIT_SUCCESS;
const int NOK = EXIT_FAILURE;

std::condition_variable terminate;
std::mutex mutex;

void onsignal(int value)
{
  terminate.notify_one();
}

int main(int argc, char** argv)
{
  std::signal(SIGINT, onsignal);

  cxxopts::Options options("voyx");

  options.set_width(80);

  options.add_options()
    ("h,help",   "Print this help")
    ("l,list",   "List available audio devices for -i and -o")
    ("t,term",   "Terminate after specified number of seconds", cxxopts::value<int>()->default_value("0"))
    ("i,input",  "Input audio device name or .wav file", cxxopts::value<std::string>()->default_value(""))
    ("o,output", "Output audio device name or .wav file", cxxopts::value<std::string>()->default_value(""));

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
    AudioProbe probe;

    std::cout << probe();

    return OK;
  }

  const std::string input = args["input"].as<std::string>();
  const std::string output = args["output"].as<std::string>();

  const int seconds = args["term"].as<int>();

  const size_t samplerate = 44100;
  const size_t framesize = 1024;
  const size_t hopsize = framesize / 32;
  const size_t buffersize = 1000;

  std::shared_ptr<Source<float>> source;
  std::shared_ptr<Sink<float>> sink;

  if (input.empty())
  {
    source = std::make_shared<NullSource>(samplerate, framesize, buffersize);
  }
  else if ($$::imatch(input, ".*.wav"))
  {
    source = std::make_shared<FileSource>(input, samplerate, framesize, buffersize);
  }
  else
  {
    source = std::make_shared<AudioSource>(input, samplerate, framesize, buffersize);
  }

  if (output.empty())
  {
    sink = std::make_shared<NullSink>(samplerate, framesize, buffersize);
  }
  else if ($$::imatch(output, ".*.wav"))
  {
    sink = std::make_shared<FileSink>(output, samplerate, framesize, buffersize);
  }
  else
  {
    sink = std::make_shared<AudioSink>(output, samplerate, framesize, buffersize);
  }

  std::shared_ptr<Plot> plot = std::make_shared<QPlot>(source->timeout());

  auto pipe = std::make_shared<TestPipeline>(plot, source, sink);
  // auto pipe = std::make_shared<BypassPipeline>(source, sink);
  // auto pipe = std::make_shared<StftPipeline>(framesize, hopsize, source, sink);

  pipe->open();

  if (seconds > 0)
  {
    pipe->start(std::chrono::seconds(seconds));
  }
  else
  {
    pipe->start();

    if (plot != nullptr)
    {
      plot->show();
    }
    else
    {
      std::unique_lock lock(mutex);
      terminate.wait(lock);
    }
  }

  pipe->close();

  return OK;
}
