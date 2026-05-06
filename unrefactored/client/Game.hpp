#pragma once
#include "vulkan/setup/Instance.hpp"
#include "vulkan/setup/ValidationLayer.hpp"
#include "vulkan/setup/Device.hpp"
#include "vulkan/setup/CommandPool.hpp"
#include "vulkan/window/Window.hpp"
#include "vulkan/window/GraphicsQueue.hpp"
#include "vulkan/window/Swapchain.hpp"
#include "vulkan/window/RenderSync.hpp"
#include "server/World.hpp"
#include "GameFolder.hpp"

class Game2
{
public:
    Instance instance;
    InstanceSettings instanceSettings;
    Window window;
    ValidationLayer validationLayer;
    DeviceSettings deviceSettings;
    Device device;
    GraphicsQueue queue;
    Swapchain swapchain;
    CommandPool commandPool;    
    RenderSync render;

    GameFolder gf;
    World world;
    
    
    std::filesystem::path projectBaseDir;
    Game2(std::filesystem::path projectBaseDir);
    ~Game2();
};
