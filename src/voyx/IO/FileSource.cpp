#include <voyx/IO/FileSource.h>

#include <voyx/Source.h>
#include <voyx/ETC/WAV.h>

FileSource::FileSource(const std::string& path, voyx_t samplerate, size_t framesize, size_t buffersize) :
  Source(samplerate, framesize, buffersize),
  path(path)
{
  frame = memory()->allocate(framesize);
}

FileSource::~FileSource()
{
  memory()->deallocate(frame);
}

void FileSource::open()
{
  WAV::read(path, data, samplerate());
}

void FileSource::close()
{
  data.clear();
}

bool FileSource::read(const size_t index, std::function<void(const voyx::vector<voyx_t> frame)> callback)
{
  const size_t offset = index * frame.size();

  for (size_t i = 0; i < frame.size(); ++i)
  {
    const size_t j = (i + offset) % data.size();

    frame[i] = data[j];
  }

  callback(frame);

  return true;
}
