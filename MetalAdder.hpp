#ifndef METAL_ADDER_HPP
#define METAL_ADDER_HPP

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>

#include <iostream>

class MetalAdder {
public:
  MTL::Device *device;
  // The compute pipeline generated from the compute kernel in the .metal shader
  // file.
  MTL::ComputePipelineState *computePipelineState;
  // The command queue used to pass commands to the device.
  MTL::CommandQueue *commandQueue;

  MetalAdder(MTL::Device *d) {
    device = d;

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

  void process(float *a, float *b, float *c, std::size_t size) {
    // commandQueue = device->newCommandQueue();
    MTL::CommandBuffer *commandBuffer = commandQueue->commandBuffer();
    MTL::ComputeCommandEncoder *computeEncoder =
        commandBuffer->computeCommandEncoder();

    computeEncoder->setComputePipelineState(
        computePipelineState); // points the compute pipeline the add_arrays
                               // function

    MTL::Buffer *bufferA = device->newBuffer(a, size * sizeof(float),
                                             MTL::ResourceStorageModeShared);
    MTL::Buffer *bufferB = device->newBuffer(b, size * sizeof(float),
                                             MTL::ResourceStorageModeShared);
    MTL::Buffer *bufferC = device->newBuffer(c, size * sizeof(float),
                                             MTL::ResourceStorageModeShared);

    computeEncoder->setBuffer(bufferA, 0, 0); // Argument 0
    computeEncoder->setBuffer(bufferB, 0, 1); // Argument 1
    computeEncoder->setBuffer(bufferC, 0, 2); // Argument 2

    NS::UInteger arrayLength = size; // length of the arrays
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

    // copy the data back for the caller
    float *resultPointer = (float *)bufferC->contents();
    for (int i = 0; i < size; i++) {
      c[i] = resultPointer[i];
    }
  };
};

#endif