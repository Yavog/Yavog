#pragma once
#include "vulkan/Header.hpp"
#include "vulkan/draw/DescriptorLayout.hpp"
#include "vulkan/window/RenderSync.hpp"
#include <cstddef>
#include <map>
#include <memory>
#include <vector>


class DescriptorInfo{
public:    
    enum InfoType{
        BUFFER,
        IMAGE,
    };
    InfoType type;

    vk::DescriptorBufferInfo bufferInfo;
    vk::DescriptorImageInfo imageInfo;
};

struct ResourceReincarnation{
    virtual DescriptorInfo getDescriptorInfo()const = 0;
};
struct Resource{
    virtual std::shared_ptr<ResourceReincarnation> getResource(size_t frameIndex)const = 0;
};


class DescriptorSet{
    struct Binding:DescriptorLayout{
        std::shared_ptr<Resource> resource;
        struct Frame{
            std::shared_ptr<ResourceReincarnation> reincarnation;
        };
        std::vector<Frame> frames;

        Binding(RenderSync& render,const DescriptorLayout& dsLayout);
    };
    std::vector<Binding> bindings;
    std::map<size_t, size_t> mappingID2Index;

    RenderSync* render = nullptr;
public:
    vk::raii::DescriptorPool descriptorPool = nullptr;
    std::vector<vk::raii::DescriptorSet> descriptorSets;


    void create(Device& device,RenderSync* render,DescriptorSetLayout& dsl,std::vector<DescriptorLayout> dsArray);
    void bind(Device& device,vk::raii::CommandBuffer& commandBuffer,RenderSync& render, class Pipeline& pipeline,uint32_t firstSet = 0);

    void setResource(size_t binding,std::shared_ptr<Resource> resource);

    ~DescriptorSet();
};

