#pragma once
#include "vulkan/draw/Descriptor.hpp"
#include "vulkan/setup/Device.hpp"
#include "vulkan/window/RenderSync.hpp"
#include "vulkan/draw/Buffer.hpp"
#include <cstddef>
#include <memory>
#include <vulkan/vulkan.hpp>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class UBO:public Resource
{
    struct Reincarnation:public ResourceReincarnation{
        Buffer buffer;
        void* buffersMapped;
        size_t size;

        virtual DescriptorInfo getDescriptorInfo()const;
    };
public:
    std::vector<std::shared_ptr<Reincarnation>> frames;

    void create(RenderSync* render,Device& device,vk::DeviceSize bufferSize);
    virtual std::shared_ptr<ResourceReincarnation> getResource(size_t frameIndex)const override;

};

