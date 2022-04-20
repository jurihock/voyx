#pragma once

#include <voyx/Header.h>
#include <voyx/IO/Source.h>

class FileSource : public Source<voyx_t>
{

public:

  FileSource(const std::string& path, voyx_t samplerate, size_t framesize, size_t buffersize);

  void open() override;
  void close() override;

  bool read(const size_t index, std::function<void(const std::vector<voyx_t>& frame)> callback) override;

private:

  const std::string path;

  std::vector<voyx_t> data;
  std::vector<voyx_t> frame;

};
