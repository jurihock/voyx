#pragma once

#include <metal_stdlib>
using namespace metal;

#include "Complex.h"

kernel void sdft(device const float* sample,
                 device voyx::complex* dft,
                 uint i [[thread_position_in_grid]])
{
  //c[i] = a[i] + b[i];
}
