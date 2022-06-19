#include <voyx/Source.h>

#ifdef VOYXMETAL
#define NS_PRIVATE_IMPLEMENTATION
#include <Foundation/Foundation.hpp>
#define MTL_PRIVATE_IMPLEMENTATION
#include <Metal/Metal.hpp>
#endif

#include <voyx/io/AudioProbe.h>
#include <voyx/io/MidiObserver.h>
#include <voyx/io/MidiProbe.h>
#include <voyx/ui/Plot.h>
#include <voyx/ui/QPlot.h>

#include <voyx/io/AudioSink.h>
#include <voyx/io/AudioSource.h>
#include <voyx/io/FileSink.h>
#include <voyx/io/FileSource.h>
#include <voyx/io/NoiseSource.h>
#include <voyx/io/NullSink.h>
#include <voyx/io/NullSource.h>
#include <voyx/io/SineSource.h>
#include <voyx/io/SweepSource.h>

#include <voyx/dsp/BypassPipeline.h>
#include <voyx/dsp/InverseSynthPipeline.h>
#include <voyx/dsp/MetalTestPipeline.h>
#include <voyx/dsp/OpenclTestPipeline.h>
#include <voyx/dsp/RobotPipeline.h>
#include <voyx/dsp/SdftTestPipeline.h>
#include <voyx/dsp/SlidingVoiceSynthPipeline.h>
#include <voyx/dsp/StftTestPipeline.h>
#include <voyx/dsp/VoiceSynthPipeline.h>

#include <cxxopts.hpp>

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
    ("b,buffer",  "Audio fifo size", cxxopts::value<int>()->default_value("100"))
    ("d,debug",   "Enable debug mode");

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

  const bool debug = args.count("debug");

  std::shared_ptr<Source<>> source;
  std::shared_ptr<Sink<>> sink;

  if (input.empty())
  {
    source = std::make_shared<NullSource>(samplerate, framesize, buffersize);
  }
  else if ($$::imatch(input, "noise"))
  {
    source = std::make_shared<NoiseSource>(0.5, samplerate, framesize, buffersize);
  }
  else if ($$::imatch(input, "sine"))
  {
    source = std::make_shared<SineSource>(0.5, concertpitch, samplerate, framesize, buffersize);
  }
  else if ($$::imatch(input, "sweep"))
  {
    source = std::make_shared<SweepSource>(0.5, std::make_pair(concertpitch / 2, concertpitch * 2), 10, samplerate, framesize, buffersize);
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

  std::shared_ptr<MidiObserver> observer = midi.empty() ? nullptr : std::make_shared<MidiObserver>(midi, concertpitch);

  #ifdef VOYXUI
  std::shared_ptr<Plot> plot = !debug ? nullptr : std::make_shared<QPlot>(source->timeout());
  #else
  std::shared_ptr<Plot> plot = nullptr;
  #endif

  const size_t dftsize = 1*1024;

  // auto pipe = std::make_shared<BypassPipeline>(source, sink);
  // auto pipe = std::make_shared<InverseSynthPipeline>(samplerate, framesize, hopsize, source, sink, observer, plot);
  // auto pipe = std::make_shared<MetalTestPipeline>(samplerate, framesize, dftsize, source, sink);
  // auto pipe = std::make_shared<OpenclTestPipeline>(samplerate, framesize, dftsize, source, sink);
  // auto pipe = std::make_shared<RobotPipeline>(samplerate, framesize, dftsize, source, sink, observer, plot);
  // auto pipe = std::make_shared<SdftTestPipeline>(samplerate, framesize, dftsize, source, sink, observer, plot);
  auto pipe = std::make_shared<SlidingVoiceSynthPipeline>(samplerate, framesize, dftsize, source, sink, observer, plot);
  // auto pipe = std::make_shared<StftTestPipeline>(samplerate, framesize, hopsize, source, sink, observer, plot);
  // auto pipe = std::make_shared<VoiceSynthPipeline>(samplerate, framesize, hopsize, source, sink, observer, plot);

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
