#pragma once

#include <voyx/Header.h>
#include <voyx/IO/Source.h>

class NullSource : public Source<float>
{

public:

  NullSource(size_t samplerate, size_t framesize, size_t buffersize);

  bool read(const size_t index, std::function<void(const std::vector<float>& frame)> callback) override;

private:

  const std::vector<float> frame;

};
