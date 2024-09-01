#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>

#include "MetalAdder.hpp"

#include <iostream>

void metal_add_arrays(MTL::Device* d, MTL::Buffer* a, MTL::Buffer* b, MTL::Buffer* c) {
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
  MTL::ComputePipelineState *computePipelineState =
      d->newComputePipelineState(addArrays, &error);
  if (computePipelineState == nullptr) {
    std::cout << "Failed to create compute pipeline." << std::endl;
    return;
  }

  // don't know what any of this really is, something that tells the GPU what to
  // do ig
  MTL::CommandQueue *commandQueue = d->newCommandQueue();
  MTL::CommandBuffer *commandBuffer = commandQueue->commandBuffer();
  MTL::ComputeCommandEncoder *computeEncoder =
      commandBuffer->computeCommandEncoder();

  computeEncoder->setComputePipelineState(
      computePipelineState); // points the compute pipeline the add_arrays
                             // function

  computeEncoder->setBuffer(a, 0, 0); // Argument 0
  computeEncoder->setBuffer(b, 0, 1); // Argument 1
  computeEncoder->setBuffer(c, 0, 2); // Argument 2

  NS::UInteger arrayLength = a->length(); // length of the arrays
  NS::UInteger threadGroupSize =
      computePipelineState->maxTotalThreadsPerThreadgroup();
  NS::UInteger numThreadgroups =
      (arrayLength + threadGroupSize - 1) / threadGroupSize;

  computeEncoder->dispatchThreadgroups(MTL::Size::Make(numThreadgroups, 1, 1),
                                       MTL::Size::Make(threadGroupSize, 1, 1));

  computeEncoder->endEncoding();
  commandBuffer->commit();
  commandBuffer->waitUntilCompleted();

//   defaultLibrary->release();
//   addArrays->release();
//   commandQueue->release();
//   commandBuffer->release();
//   computeEncoder->release();
}

// Implementation 1: not reusable at all
// Implementation 2: easy to call, not reusable
// Implementation 3: instantiate once, fully reusable

// #define I1
#define I2
#define I3

int main() {
  std::cout << "Hi" << std::endl;

  MTL::Device *d = MTL::CreateSystemDefaultDevice();

#ifdef I1
  std::array<float, 4> a1 = {1.0f, 2.0f, 3.0f, 4.0f};
  std::array<float, 4> b1 = {5.0f, 6.0f, 7.0f, 8.0f};
  std::array<float, 4> c1;

  NS::Error *error = nullptr;

  MTL::Library *defaultLibrary = d->newDefaultLibrary();
  if (defaultLibrary == nullptr) {
    std::cout << "Failed to find the default library." << std::endl;
    return -1;
  }

  auto str = NS::String::string("add_arrays", NS::ASCIIStringEncoding);
  MTL::Function *addArrays = defaultLibrary->newFunction(str);
  if (addArrays == nullptr) {
    std::cout << "Failed to create function." << std::endl;
    return -1;
  }

  // build compute pipeline (partial)
  MTL::ComputePipelineState *computePipelineState =
      d->newComputePipelineState(addArrays, &error);
  if (computePipelineState == nullptr) {
    std::cout << "Failed to create compute pipeline." << std::endl;
    return -1;
  }

  // don't know what any of this really is, something that tells the GPU what to
  // do ig
  MTL::CommandQueue *commandQueue = d->newCommandQueue();
  MTL::CommandBuffer *commandBuffer = commandQueue->commandBuffer();
  MTL::ComputeCommandEncoder *computeEncoder =
      commandBuffer->computeCommandEncoder();

  computeEncoder->setComputePipelineState(
      computePipelineState); // points the compute pipeline the add_arrays
                             // function

  MTL::Buffer *bufferA1 = d->newBuffer(a1.data(), sizeof(a1), MTL::ResourceStorageModeShared);
  MTL::Buffer *bufferB1 = d->newBuffer(b1.data(), sizeof(b1), MTL::ResourceStorageModeShared);
  MTL::Buffer *bufferC1 = d->newBuffer(c1.data(), sizeof(c1), MTL::ResourceStorageModeShared);
  // still use sizeof() not .size()

  computeEncoder->setBuffer(bufferA1, 0, 0); // Argument 0
  computeEncoder->setBuffer(bufferB1, 0, 1); // Argument 1
  computeEncoder->setBuffer(bufferC1, 0, 2); // Argument 2

  NS::UInteger arrayLength = a1.size(); // length of the arrays
  NS::UInteger threadGroupSize =
      computePipelineState->maxTotalThreadsPerThreadgroup();
  NS::UInteger numThreadgroups =
      (arrayLength + threadGroupSize - 1) / threadGroupSize;

  computeEncoder->dispatchThreadgroups(MTL::Size::Make(numThreadgroups, 1, 1),
                                       MTL::Size::Make(threadGroupSize, 1, 1));

  computeEncoder->endEncoding();
  commandBuffer->commit();
  commandBuffer->waitUntilCompleted();

  float *resultPointer1 = (float *)bufferC1->contents();
  for (int i = 0; i < a1.size(); i++) {
    c1[i] = resultPointer1[i];
  }
  for (float f : c1) {
    std::cout << f << " ";
  }
  std::cout << std::endl; // result is correct

#endif

#ifdef I2
  std::array<float, 4> a2 = {1.0f, 2.0f, 3.0f, 4.0f};
  std::array<float, 4> b2 = {5.0f, 6.0f, 7.0f, 8.0f};
  std::array<float, 4> c2;

  MTL::Buffer *bufferA2 = d->newBuffer(a2.data(), sizeof(a2), MTL::ResourceStorageModeShared);
  MTL::Buffer *bufferB2 = d->newBuffer(b2.data(), sizeof(b2), MTL::ResourceStorageModeShared);
  MTL::Buffer *bufferC2 = d->newBuffer(c2.data(), sizeof(c2), MTL::ResourceStorageModeShared);

  metal_add_arrays(d, bufferA2, bufferB2, bufferC2);

  float *resultPointer2 = (float *)bufferC2->contents();
  for (int i = 0; i < a2.size(); i++) {
    c2[i] = resultPointer2[i];
  }
  for (float f : c2) {
    std::cout << f << " ";
  }
  std::cout << std::endl;

#endif

#ifdef I3
  std::array<float, 4> a3 = {1.0f, 2.0f, 3.0f, 4.0f};
  std::array<float, 4> b3 = {5.0f, 6.0f, 7.0f, 8.0f};
  std::array<float, 4> c3;

  MTL::Buffer *bufferA3 = d->newBuffer(a3.data(), sizeof(a3), MTL::ResourceStorageModeShared);
  MTL::Buffer *bufferB3 =  d->newBuffer(b3.data(), sizeof(b3), MTL::ResourceStorageModeShared);
  MTL::Buffer *bufferC3 = d->newBuffer(c3.data(), sizeof(c3), MTL::ResourceStorageModeShared);

  MetalAdder *adder = new MetalAdder(d);
  adder->run(bufferA3, bufferB3, bufferC3);

  float *resultPointer3 = (float *)bufferC3->contents();
  for (int i = 0; i < a3.size(); i++) {
    c3[i] = resultPointer3[i];
  }
  for (float f : c3) {
    std::cout << f << " ";
  }
  std::cout << std::endl;

#endif

  return 0;
}