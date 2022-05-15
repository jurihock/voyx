#include <metal_stdlib>

#include "Metal.h"

using namespace metal;

kernel void add_arrays(device const voyx::complex* a,
                       device const voyx::complex* b,
                       device voyx::complex* c,
                       uint i [[thread_position_in_grid]])
{
  c[i] = a[i] + b[i];
}
