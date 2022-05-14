#include <metal_stdlib>

using namespace metal;

kernel void add_arrays(device const float* a,
                       device const float* b,
                       device float* c,
                       uint index [[thread_position_in_grid]])
{
  c[index] = a[index] + b[index];
}
