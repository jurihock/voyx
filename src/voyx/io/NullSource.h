#pragma once

#include <voyx/Header.h>
#include <voyx/IO/Source.h>

class NullSource : public Source<voyx_t>
{

public:

  NullSource(voyx_t samplerate, size_t framesize, size_t buffersize);

  bool read(const size_t index, std::function<void(const voyx::vector<voyx_t> frame)> callback) override;

private:

  const std::vector<voyx_t> frame;

};
