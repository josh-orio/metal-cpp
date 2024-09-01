#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>

#include "MetalAdder.hpp"

#include <iostream>

void metal_add_arrays(MTL::Device *d, float *a, float *b, float *c,
                      std::size_t size) {
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

  MTL::Buffer *bufferA = d->newBuffer(a, size * sizeof(float),
                                           MTL::ResourceStorageModeShared);
  MTL::Buffer *bufferB = d->newBuffer(b, size * sizeof(float),
                                           MTL::ResourceStorageModeShared);
  MTL::Buffer *bufferC = d->newBuffer(c, size * sizeof(float),
                                           MTL::ResourceStorageModeShared);

  computeEncoder->setBuffer(bufferA, 0, 0); // Argument 0
  computeEncoder->setBuffer(bufferB, 0, 1); // Argument 1
  computeEncoder->setBuffer(bufferC, 0, 2); // Argument 2

  NS::UInteger arrayLength = size; // length of the arrays
  NS::UInteger threadGroupSize =
      computePipelineState->maxTotalThreadsPerThreadgroup();
  NS::UInteger numThreadgroups =
      (arrayLength + threadGroupSize - 1) / threadGroupSize;

  computeEncoder->dispatchThreadgroups(MTL::Size::Make(numThreadgroups, 1, 1),
                                       MTL::Size::Make(threadGroupSize, 1, 1));

  computeEncoder->endEncoding();
  commandBuffer->commit();
  commandBuffer->waitUntilCompleted();

  // copy the data back for the caller
  float *resultPointer = (float *)bufferC->contents();
  for (int i = 0; i < size; i++) {
    c[i] = resultPointer[i];
  }
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

  MTL::Buffer *bufferA1 =
      d->newBuffer(a1.data(), sizeof(a1), MTL::ResourceStorageModeShared);
  MTL::Buffer *bufferB1 =
      d->newBuffer(b1.data(), sizeof(b1), MTL::ResourceStorageModeShared);
  MTL::Buffer *bufferC1 =
      d->newBuffer(c1.data(), sizeof(c1), MTL::ResourceStorageModeShared);
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

  metal_add_arrays(d, a2.data(), b2.data(), c2.data(), a2.size());

  for (float c : c2) {
    std::cout << c << " ";
  }
  std::cout << std::endl;

#endif

#ifdef I3
  std::array<float, 4> a3 = {1.0f, 2.0f, 3.0f, 4.0f};
  std::array<float, 4> b3 = {5.0f, 6.0f, 7.0f, 8.0f};
  std::array<float, 4> c3 = {};

  MetalAdder *adder = new MetalAdder(d);
  adder->process(a3.data(), b3.data(), c3.data(), a3.size());

  for (float c : c3) {
    std::cout << c << " ";
  }
  std::cout << std::endl;

#endif

  return 0;
}