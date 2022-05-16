#include <voyx/IO/FileSink.h>

#include <voyx/Source.h>
#include <voyx/ETC/WAV.h>

FileSink::FileSink(const std::string& path, voyx_t samplerate, size_t framesize, size_t buffersize) :
  Sink(samplerate, framesize, buffersize),
  path(path)
{
}

void FileSink::open()
{
  data.clear();
}

void FileSink::close()
{
  WAV::write(path, data, samplerate());
}

bool FileSink::write(const size_t index, const voyx::vector<voyx_t> frame)
{
  const size_t oldsize = data.size();
  const size_t newsize = oldsize + frame.size();

  data.resize(newsize);

  for (size_t i = 0; i < frame.size(); ++i)
  {
    data[i + oldsize] = frame[i];
  }

  return true;
}
