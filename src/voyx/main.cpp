#include <voyx/Voyx.h>
#include <voyx/IO/Probe.h>

#include <cxxopts/cxxopts.hpp>

#include <cstdlib>

const int OK = EXIT_SUCCESS;
const int NOK = EXIT_FAILURE;

INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv)
{
  cxxopts::Options options("voyx");

  options.add_options()
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

  return OK;
}
