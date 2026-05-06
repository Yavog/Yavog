#include <filesystem>
#include <iostream>
#include "App.hpp"
#include "GLFW/glfw3.h"
#include "vulkan/window/DepthBuffer.hpp"

App::App(std::filesystem::path projectDir):projectDir(projectDir){
}


bool App::run(){    
    std::cout<<projectDir<<std::endl;
    while (vulkan.window.update()) {
        glfwPollEvents();

        ImageIndex imageIndex;
        if(!vulkan.render.begin(vulkan.window, vulkan.swapchain, vulkan.commandPool, &vulkan.depthBuffer,imageIndex))
            continue;
        auto& CB = vulkan.render.getCommandBuffer();
        CB.begin();
        vulkan.swapchain.beginRendering(CB, imageIndex, &vulkan.depthBuffer);
       
        vulkan.swapchain.endRendering(CB, imageIndex);
        CB.end();
        vulkan.render.end(vulkan.window, vulkan.swapchain, vulkan.commandPool, &vulkan.depthBuffer, imageIndex);

    }
    return false;
}