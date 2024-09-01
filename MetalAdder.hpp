#ifndef METAL_ADDER_HPP
#define METAL_ADDER_HPP

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>

class MetalAdder {
public:
  MTL::Device *device;

  // The compute pipeline generated from the compute kernel in the .metal shader file.
  MTL::ComputePipelineState *computePipelineState;

  // The command queue used to pass commands to the device.
  MTL::CommandQueue *commandQueue;

  MetalAdder(MTL::Device *d);
  ~MetalAdder();

  void run(MTL::Buffer *a, MTL::Buffer *b, MTL::Buffer *c);
};

#endif