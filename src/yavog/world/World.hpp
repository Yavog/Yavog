#pragma once

#include "client/Camera.hpp"
#include "vulkan/Vulkan.hpp"
#include "vulkan/draw/DescriptorLayout.hpp"
#include "vulkan/draw/Pipeline.hpp"
#include "vulkan_old/Image.hpp"

class World{
public:
    Image image;
    DescriptorSetLayout dsLayout;
    DescriptorSet ds;
    Camera camera;
    Pipeline pipeline;
    
    // static const size_t range = 3;
    // Chunk chunk[range][range][range];

    void init(Vulkan& vulkan,std::filesystem::path projectBaseDir){

        image.create(&vulkan.render,vulkan.commandPool,projectBaseDir/"assets"/"texture.jpg");
        
        camera.create(vulkan.device,&vulkan.render);
        dsLayout.create(vulkan.device,{
            DescriptorLayout(0,vk::ShaderStageFlagBits::eVertex  ,vk::DescriptorType::eUniformBuffer),
            //DescriptorLayout(1,vk::ShaderStageFlagBits::eFragment,vk::DescriptorType::eCombinedImageSampler),
        });
        ds.create(vulkan.device,&vulkan.render,dsLayout,{
            DescriptorLayout(0,vk::ShaderStageFlagBits::eVertex  ,vk::DescriptorType::eUniformBuffer),
            //Descriptor(1,vk::ShaderStageFlagBits::eFragment,image),
        });
        ds.setResource(0, camera.ubo);

        ;
        pipeline.create(&vulkan.render,vulkan.device,
            projectBaseDir/"bin"/"shaders"/"slang.spv",
            "vertMain","fragMain",
            vulkan.swapchain, dsLayout,vulkan.depthBuffer);

        // for (size_t x = 0; x < range; x++){
        //     for (size_t y = 0; y < range; y++){
        //         for (size_t z = 0; z < range; z++){
        //             chunk[x][y][z].create();
        //         }
        //     }   
        // }
    }
    void draw(Vulkan& vulkan,CommandBuffer& CB,uint32_t imageIndex){
        uint32_t frameIndex = vulkan.render.getFrameIndex();
        float aspectRatio = static_cast<float>(vulkan.swapchain.swapChainExtent.width) / static_cast<float>(vulkan.swapchain.swapChainExtent.height);

        bool zoom = glfwGetKey(vulkan.window,GLFW_KEY_C) == GLFW_PRESS;
        camera.updateUniformBuffer(frameIndex,aspectRatio, zoom,camera.pos,camera.forward);

        auto& commandBuffer = CB.commandBuffer;
        
        {
            commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline.graphicsPipeline);
            commandBuffer.setViewport(0, vk::Viewport{
                .x = 0.0f,
                .y = 0.0f,
                .width = static_cast<float>(vulkan.swapchain.swapChainExtent.width),
                .height = static_cast<float>(vulkan.swapchain.swapChainExtent.height),
                .minDepth = 0.0f,
                .maxDepth = 1.0f,
            });
            commandBuffer.setScissor(0, vk::Rect2D{
                .offset = vk::Offset2D{.x = 0,.y = 0},
                .extent = vulkan.swapchain.swapChainExtent,
            });
            
            ds.bind(vulkan.device,commandBuffer,vulkan.render,pipeline);
    
            // for (size_t x = 0; x < range; x++){
            //     for (size_t y = 0; y < range; y++){
            //         for (size_t z = 0; z < range; z++){
            //             chunk[x][y][z].draw(CB);
            //         }
            //     }
            // }
        }
    }
};