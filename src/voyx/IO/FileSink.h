#pragma once

#include <voyx/Header.h>
#include <voyx/IO/Sink.h>

class FileSink : public Sink<voyx_t>
{

public:

  FileSink(const std::string& path, voyx_t samplerate, size_t framesize, size_t buffersize);

  void open() override;
  void close() override;

  bool write(const size_t index, const std::vector<voyx_t>& frame) override;

private:

  const std::string path;

  std::vector<voyx_t> data;

};
