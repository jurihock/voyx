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
#include <voyx/IO/MidiProbe.h>
#include <voyx/IO/MidiObserver.h>
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
    ("h,help",    "Print this help")
    ("l,list",    "List available devices for -m, -i and -o")
    ("m,midi",    "Input MIDI device name", cxxopts::value<std::string>()->default_value(""))
    ("i,input",   "Input audio device or .wav file name", cxxopts::value<std::string>()->default_value(""))
    ("o,output",  "Output audio device or .wav file name", cxxopts::value<std::string>()->default_value(""))
    ("s,sec",     "Abort after specified number of seconds", cxxopts::value<int>()->default_value("0"))
    ("a,a4",      "Concert pitch in hertz", cxxopts::value<voyx_t>()->default_value("440"))
    ("r,sr",      "Sample rate in hertz", cxxopts::value<voyx_t>()->default_value("44100"))
    ("w,window",  "STFT window size", cxxopts::value<int>()->default_value("1024"))
    ("v,overlap", "STFT window overlap", cxxopts::value<int>()->default_value("4"))
    ("b,buffer",  "Audio IO buffer size", cxxopts::value<int>()->default_value("1000"));

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
    AudioProbe audio;
    MidiProbe midi;

    std::stringstream result;

    result << "~ DETECTED AUDIO DEVICES ~"
           << std::endl
           << std::endl
           << audio();

    result << std::endl;

    result << "~ DETECTED MIDI DEVICES ~"
           << std::endl
           << std::endl
           << midi();

    std::cout << result.str();

    return OK;
  }

  const std::string midi = args["midi"].as<std::string>();
  const std::string input = args["input"].as<std::string>();
  const std::string output = args["output"].as<std::string>();

  const int seconds = std::abs(args["sec"].as<int>());

  const voyx_t concertpitch = std::abs(args["a4"].as<voyx_t>());
  const voyx_t samplerate = std::abs(args["sr"].as<voyx_t>());
  const size_t framesize = std::abs(args["window"].as<int>());
  const size_t hopsize = framesize / std::abs(args["overlap"].as<int>());
  const size_t buffersize = std::abs(args["buffer"].as<int>());

  std::shared_ptr<Source<>> source;
  std::shared_ptr<Sink<>> sink;

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

  std::shared_ptr<MidiObserver> observer = std::make_shared<MidiObserver>(midi, concertpitch);

  std::shared_ptr<Plot> plot = std::make_shared<QPlot>(source->timeout());

  // auto pipe = std::make_shared<BypassPipeline>(source, sink);
  // auto pipe = std::make_shared<StftPipeline>(samplerate, framesize, hopsize, source, sink);
  auto pipe = std::make_shared<TestPipeline>(samplerate, framesize, hopsize, source, sink, observer, plot);

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
