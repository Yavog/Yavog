#pragma once
#include "vulkan/setup/Instance.hpp"
#include "vulkan/setup/ValidationLayer.hpp"
#include "vulkan/setup/Device.hpp"
#include "vulkan/setup/CommandPool.hpp"
#include "vulkan/window/Window.hpp"
#include "vulkan/window/GraphicsQueue.hpp"
#include "vulkan/window/Swapchain.hpp"
#include "vulkan/window/RenderSync.hpp"
#include "vulkan/window/DepthBuffer.hpp"

class Vulkan{
public:
    Instance instance;
    InstanceSettings instanceSettings;
    ValidationLayer validationLayer;
    Window window;
    DeviceSettings deviceSettings;
    Device device;
    GraphicsQueue queue;
    Swapchain swapchain;
    CommandPool commandPool;    
    RenderSync render;
    DepthBuffer depthBuffer;

    Vulkan();
};