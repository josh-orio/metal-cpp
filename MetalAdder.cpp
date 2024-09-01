#include "MetalAdder.hpp"
#include <iostream>

MetalAdder::MetalAdder(MTL::Device *d) {
  device = d;
  
  NS::Error *error = nullptr;

  MTL::Library *defaultLibrary = device->newDefaultLibrary();
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
  computePipelineState = device->newComputePipelineState(addArrays, &error);
  addArrays->release();
  if (computePipelineState == nullptr) {
    std::cout << "Failed to create compute pipeline state." << std::endl;
    return;
  }

  commandQueue = device->newCommandQueue();
  if (commandQueue == nullptr) {
    std::cout << "Failed to find command queue." << std::endl;
    return;
  }
}

void MetalAdder::run(MTL::Buffer *a, MTL::Buffer *b, MTL::Buffer *c) {
  // commandQueue = device->newCommandQueue();
  MTL::CommandBuffer *commandBuffer = commandQueue->commandBuffer();
  MTL::ComputeCommandEncoder *computeEncoder = commandBuffer->computeCommandEncoder();

  computeEncoder->setComputePipelineState(computePipelineState); // points the compute pipeline the add_arrays function

  computeEncoder->setBuffer(a, 0, 0); // Argument 0
  computeEncoder->setBuffer(b, 0, 1); // Argument 1
  computeEncoder->setBuffer(c, 0, 2); // Argument 2

  NS::UInteger arrayLength = 4; // length of the arrays
  NS::UInteger threadGroupSize = computePipelineState->maxTotalThreadsPerThreadgroup();
  NS::UInteger numThreadgroups = (arrayLength + threadGroupSize - 1) / threadGroupSize;

  computeEncoder->dispatchThreadgroups(MTL::Size::Make(numThreadgroups, 1, 1),
                                       MTL::Size::Make(threadGroupSize, 1, 1));

  computeEncoder->endEncoding();
  commandBuffer->commit();
  commandBuffer->waitUntilCompleted();
};

MetalAdder::~MetalAdder() {
  computePipelineState->release();
  commandQueue->release();
}
