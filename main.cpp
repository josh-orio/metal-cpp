#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#include <Metal/Metal.hpp>
#include <Foundation/Foundation.hpp>


#include <iostream>
#include <filesystem>
#include <fstream>


int main() {
    std::cout << "Hi" << std::endl;

    MTL::Device* d = MTL::CreateSystemDefaultDevice();

    std::string fn = "../test.metal"; // executable is in ./build
    std::vector<char> bin(std::filesystem::file_size(fn));
    std::ifstream(fn, std::ifstream::binary).read(bin.data(), bin.size());
    std::string shader (bin.begin(), bin.end());
    // all this logic just loads in test.metal as an std::string

    NS::Error* error = nullptr;

    // compile the .metal file content to a library
    MTL::Library* library = d->newLibrary(NS::String::string(shader.c_str(), NS::UTF8StringEncoding), nullptr, &error);
    if (!library) {
        std::cout << "Failed to create library" << std::endl;
        return -1;
    }
    else {
        std::cout << "Library built!" << std::endl;
    }

    // create a function from the library ("add_arrays")
    MTL::Function *function = library->newFunction(NS::String::string("add_arrays", NS::UTF8StringEncoding));
    if (!function) {
        std::cout << "Failed to create function" << std::endl;
        return -1;
    }
    else {
        std::cout << "Function created!" << std::endl;
    }
    
    // build compute pipeline (partial)
    MTL::ComputePipelineState* computePipelineState = d->newComputePipelineState(function, &error);
    if (!computePipelineState) {
        std::cout << "Failed to create compute pipeline state: " << error->localizedDescription()->utf8String() << std::endl;
        return -1;
    }
    else {
        std::cout << "Pipeline created!" << std::endl;
    }

    // don't know what any of this really is, something that tells the GPU what to do ig
    MTL::CommandQueue* commandQueue = d->newCommandQueue();
    MTL::CommandBuffer* commandBuffer = commandQueue->commandBuffer();
    MTL::ComputeCommandEncoder *computeEncoder = commandBuffer->computeCommandEncoder();

    computeEncoder->setComputePipelineState(computePipelineState); // points the compute pipeline the add_arrays function

    std::array<float, 4> a = {1.0f, 2.0f, 3.0f, 4.0f};
    std::array<float, 4> b = {5.0f, 6.0f, 7.0f, 8.0f};
    std::array<float, 4> c;
    // don't like c-style arrs :)

    MTL::Buffer *bufferA = d->newBuffer(a.data(), sizeof(a), MTL::ResourceStorageModeShared);
    MTL::Buffer *bufferB = d->newBuffer(b.data(), sizeof(b), MTL::ResourceStorageModeShared);
    MTL::Buffer *bufferC = d->newBuffer(c.data(), sizeof(c), MTL::ResourceStorageModeShared);
    // still use sizeof() not .size()


    computeEncoder->setBuffer(bufferA, 0, 0); // Argument 0
    computeEncoder->setBuffer(bufferB, 0, 1); // Argument 1
    computeEncoder->setBuffer(bufferC, 0, 2); // Argument 2


    NS::UInteger arrayLength = 4; // length of the arrays
    NS::UInteger threadGroupSize = computePipelineState->maxTotalThreadsPerThreadgroup();
    NS::UInteger numThreadgroups = (arrayLength + threadGroupSize - 1) / threadGroupSize;

    computeEncoder->dispatchThreadgroups(MTL::Size::Make(numThreadgroups, 1, 1),
                                        MTL::Size::Make(threadGroupSize, 1, 1));

    computeEncoder->endEncoding();
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();

    float* resultPointer = (float*)bufferC->contents();

    std::cout << "[ ";
    for (int i = 0; i < arrayLength; i++) {
        std::cout << resultPointer[i] << " ";
    }
    std::cout << "]" << std::endl; // result is correct


    return 0;
}