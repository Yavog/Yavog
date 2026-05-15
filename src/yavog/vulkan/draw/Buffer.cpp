#include "Buffer.hpp"
#include <cassert>

uint32_t Buffer::findMemoryType(Device& device,uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    vk::PhysicalDeviceMemoryProperties memProperties = device.physicalDevice.getMemoryProperties();
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}
void Buffer::copyBuffer(CommandPool& commandPool,Buffer& srcBuffer, Buffer& dstBuffer, vk::DeviceSize size) {
    CommandBuffer commandBuffer(commandPool);
    commandBuffer.beginSingleTimeCommands();
    commandBuffer.commandBuffer.copyBuffer(srcBuffer.buffer,dstBuffer.buffer,vk::BufferCopy{
        .srcOffset = 0,
        .dstOffset = 0,
        .size      = size,
    });
    commandBuffer.endSingleTimeCommands(commandPool);
}
Buffer::~Buffer(){
    if(render)
        render->trash(std::move(buffer),std::move(bufferMemory));
}
void Buffer::create(RenderSync* render,Device& device,vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) {
    assert(render);
    if(this->render)
        assert(this->render == render);
    this->render = render;

    if(render)
        render->trash(std::move(buffer),std::move(bufferMemory));
    

    vk::BufferCreateInfo bufferInfo{ 
        .size = size, 
        .usage = usage, 
        .sharingMode = vk::SharingMode::eExclusive 
    };
    buffer = vk::raii::Buffer(device.device, bufferInfo);
    vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();
    vk::MemoryAllocateInfo allocInfo{ 
        .allocationSize = memRequirements.size, 
        .memoryTypeIndex = findMemoryType(device,memRequirements.memoryTypeBits, properties) 
    };
    bufferMemory = vk::raii::DeviceMemory(device.device, allocInfo);
    buffer.bindMemory(*bufferMemory, 0);
}
void Buffer::createAndUpload(RenderSync* render,CommandPool& commandPool,const void* ptr,size_t size,vk::Flags<vk::BufferUsageFlagBits> usage){
    Device& device = commandPool.getDevice();
    //create StagingBuffer
    Buffer stagingBuffer;
    {
        auto usage = vk::BufferUsageFlagBits::eTransferSrc;
        auto properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
        stagingBuffer.create(render,device,size, usage,properties);

        void* dataStaging = stagingBuffer.bufferMemory.mapMemory(0, size);
        memcpy(dataStaging, ptr ,size);
        stagingBuffer.bufferMemory.unmapMemory();
    }
    usage |= vk::BufferUsageFlagBits::eTransferDst;
    auto properties = vk::MemoryPropertyFlagBits::eDeviceLocal;
    create(render,device,size, usage,properties);
    copyBuffer(commandPool,stagingBuffer, *this, size);
}