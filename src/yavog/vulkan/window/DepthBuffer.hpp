#pragma once
#include "vulkan/window/RenderSync.hpp"
#include "vulkan_old/Image.hpp"
#include "vulkan/window/Swapchain.hpp"


//TODO: look into stencil tests
class DepthBuffer
{
    static vk::Format findSupportedFormat(Device& device,const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
    static bool hasStencilComponent(vk::Format format);
    static vk::Format findDepthFormat(Device& device);

    RenderSync* render = nullptr;
public:
    
    Image image;
    vk::Format depthFormat;

    void create(RenderSync* render,CommandPool& pool,Swapchain& swapchain);
    ~DepthBuffer();
};
