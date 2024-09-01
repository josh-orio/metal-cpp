#ifndef METAL_ADDER_HPP
#define METAL_ADDER_HPP

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>

#include <iostream>

class MetalAdder {
public:
  // The compute pipeline generated from the compute kernel in the .metal shader
  // file.
  MTL::ComputePipelineState *computePipelineState;
  // The command queue used to pass commands to the device.
  MTL::CommandQueue *commandQueue;

  MetalAdder(MTL::Device *d) {
    NS::Error *error = nullptr;

    MTL::Library *defaultLibrary = d->newDefaultLibrary();
    if (defaultLibrary == nullptr) {
      std::cout << "Failed to find the default library." << std::endl;
      return;
    }

    auto str = NS::String::string("add_arrays", NS::ASCIIStringEncoding);
    MTL::Function *addArrays = defaultLibrary->newFunction(str);
    if (addArrays == nullptr) {
      std::cout << "Failed to create function." << std::endl;
      return;
    }

    // build compute pipeline (partial)
    computePipelineState = d->newComputePipelineState(addArrays, &error);
    addArrays->release();
    if (computePipelineState == nullptr) {
      std::cout << "Failed to create compute pipeline state." << std::endl;
      return;
    }

    commandQueue = d->newCommandQueue();
    if (commandQueue == nullptr) {
      std::cout << "Failed to find command queue." << std::endl;
      return;
    }
  };
  
  ~MetalAdder() {
    computePipelineState->release();
    commandQueue->release();
  };

  void process(MTL::Buffer *a, MTL::Buffer *b, MTL::Buffer *c) {
    // commandQueue = device->newCommandQueue();
    MTL::CommandBuffer *commandBuffer = commandQueue->commandBuffer();
    MTL::ComputeCommandEncoder *computeEncoder =
        commandBuffer->computeCommandEncoder();

    computeEncoder->setComputePipelineState(
        computePipelineState); // points the compute pipeline the add_arrays
                               // function

    computeEncoder->setBuffer(a, 0, 0); // Argument 0
    computeEncoder->setBuffer(b, 0, 1); // Argument 1
    computeEncoder->setBuffer(c, 0, 2); // Argument 2

    NS::UInteger arrayLength = 4; // length of the arrays
    NS::UInteger threadGroupSize =
        computePipelineState->maxTotalThreadsPerThreadgroup();
    NS::UInteger numThreadgroups =
        (arrayLength + threadGroupSize - 1) / threadGroupSize;

    computeEncoder->dispatchThreadgroups(
        MTL::Size::Make(numThreadgroups, 1, 1),
        MTL::Size::Make(threadGroupSize, 1, 1));

    computeEncoder->endEncoding();
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();
  };
};

#endif