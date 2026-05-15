#include <cassert>
#include <filesystem>
#include <iostream>
#include <memory>
#include "App.hpp"
#include "GLFW/glfw3.h"
#include "glm/ext/vector_float2.hpp"
#include "gui/GuiAsset.hpp"
#include "gui/GuiScreen.hpp"
#include "vulkan/window/DepthBuffer.hpp"

App* App::app = nullptr;

App::App(std::filesystem::path projectDir):projectDir(projectDir){
    assert(!app);
    app = this;
}
App::~App(){
    assert(app);
    app = nullptr;
}


bool App::run(){    
    std::cout<<projectDir<<std::endl;
    
    guiSystem->create(projectDir);
    vulkan.window.inputHandler = guiSystem;
    
    while (vulkan.window.update()) {
        
        ImageIndex imageIndex;
        if(!vulkan.render.begin(vulkan.window, vulkan.swapchain, vulkan.commandPool, &vulkan.depthBuffer,imageIndex))
            continue;
        auto& CB = vulkan.render.getCommandBuffer();
        CB.begin();
        vulkan.swapchain.beginRendering(CB, imageIndex, &vulkan.depthBuffer);
        
        CB.commandBuffer.setViewport(0, vk::Viewport{
            .x = 0.0f,
            .y = 0.0f,
            .width = static_cast<float>( vulkan.swapchain.swapChainExtent.width),
            .height = static_cast<float>(vulkan.swapchain.swapChainExtent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        });
        CB.commandBuffer.setScissor(0, vk::Rect2D{
            .offset = vk::Offset2D{.x = 0,.y = 0},
            .extent = vulkan.swapchain.swapChainExtent,
        });
        
        glfwPollEvents();

        fpsCounter.update();
        guiSystem->draw(CB);
        client.update();
        server.update();

        vulkan.swapchain.endRendering(CB, imageIndex);
        CB.end();
        vulkan.render.end(vulkan.window, vulkan.swapchain, vulkan.commandPool, &vulkan.depthBuffer, imageIndex);

    }
    return false;
}