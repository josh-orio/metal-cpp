#include <metal_stdlib>
using namespace metal;

// add two arrays
kernel void add_arrays(device const float* a [[buffer(0)]],
                       device const float* b [[buffer(1)]],
                       device float* c [[buffer(2)]],
                       uint id [[thread_position_in_grid]]) {

    c[id] = a[id] + b[id];
}

kernel void sub_arrays(device const float* a [[buffer(0)]],
                       device const float* b [[buffer(1)]],
                       device float* c [[buffer(2)]],
                       uint id [[thread_position_in_grid]]) {

    c[id] = a[id] - b[id];
}