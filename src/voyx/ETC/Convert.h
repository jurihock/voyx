#pragma once

#include <voyx/Header.h>

namespace $$
{
  template<class T>
  std::constspan<T> constspan(std::span<T> value) { return value; }
}

namespace $$
{
  template<typename T>
  struct typeofvalue { typedef T type; };

  template<typename T>
  struct typeofvalue<std::complex<T>> { typedef T type; };
}

#include <voyx/ETC/Convert.DFT.h>
#include <voyx/ETC/Convert.MIDI.h>
#include <voyx/ETC/Convert.Interpolation.h>
#include <voyx/ETC/Convert.String.h>
#include <voyx/ETC/Convert.Type.h>
