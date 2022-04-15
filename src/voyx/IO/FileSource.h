#pragma once

#include <voyx/IO/Source.h>

class FileSource : public Source<float>
{

public:

  FileSource(const std::string& path, size_t samplerate, size_t framesize, size_t buffersize);

  void open() override;
  void close() override;

  bool read(const size_t index, std::function<void(const std::vector<float>& frame)> callback) override;

private:

  const std::string path;

  std::vector<float> data;
  std::vector<float> frame;

};
