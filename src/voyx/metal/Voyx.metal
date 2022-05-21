#include "SDFT.metal"

kernel void bypass(device const float* input,
                   device float* output,
                   uint i [[thread_position_in_grid]])
{
  output[i] = input[i];
}
