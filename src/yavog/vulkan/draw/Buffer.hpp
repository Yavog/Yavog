#pragma once
#include "vulkan/window/RenderSync.hpp"

//NOTE: there are only 4096 max memory allocations, split bigger buffers into smaller ones with offset. (TODO: custom allocator)
//NOTE: IndexBuffer and VertexBuffer in one, is more cache friendly.
//TODO: learn more about "aliasing" in Vulkan

class Buffer{
    RenderSync* render = nullptr;
public:
    vk::raii::DeviceMemory bufferMemory = nullptr;
    vk::raii::Buffer       buffer       = nullptr;

    ~Buffer();

    static uint32_t findMemoryType(Device& device,uint32_t typeFilter, vk::MemoryPropertyFlags properties);
    static void copyBuffer(CommandPool& commandPool,Buffer& srcBuffer, Buffer& dstBuffer, vk::DeviceSize size);

    void create(RenderSync* render,Device& device,vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
    void createAndUpload(RenderSync* render,CommandPool& commandPool,const void* ptr,size_t size,vk::Flags<vk::BufferUsageFlagBits> usage);
};