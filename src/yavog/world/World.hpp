#pragma once

#include "glm/ext/vector_float3.hpp"
#include "yavog/client/Camera.hpp"
#include "yavog/vulkan/Vulkan.hpp"
#include "yavog/vulkan/draw/DescriptorLayout.hpp"
#include "yavog/vulkan/draw/Pipeline.hpp"
#include "yavog/vulkan/draw/PushContant.hpp"
#include "yavog/vulkan_old/Image.hpp"
#include <memory>

class World{
public:
    std::shared_ptr<Image> image = std::make_shared<Image>();
    DescriptorSetLayout dsLayout;
    DescriptorSet ds;
    Camera camera;
    Pipeline pipeline;
    PushConstant pushConstant;

    struct WorldPushConstant{
        glm::vec3 position;
    };
    // static const size_t range = 3;
    // Chunk chunk[range][range][range];

    void init(Vulkan& vulkan,std::filesystem::path projectBaseDir){
        
        image->create(&vulkan.render,vulkan.commandPool,
            projectBaseDir/"assets"/"block"/"textureAtlas.png");
        
        camera.create(vulkan.device,&vulkan.render);
        dsLayout.create(vulkan.device,{
            DescriptorLayout(0,vk::ShaderStageFlagBits::eVertex  ,vk::DescriptorType::eUniformBuffer),
            DescriptorLayout(1,vk::ShaderStageFlagBits::eFragment,vk::DescriptorType::eCombinedImageSampler),
        });
        ds.create(vulkan.device,&vulkan.render,dsLayout,{
            DescriptorLayout(0,vk::ShaderStageFlagBits::eVertex  ,vk::DescriptorType::eUniformBuffer),
            DescriptorLayout(1,vk::ShaderStageFlagBits::eFragment,vk::DescriptorType::eCombinedImageSampler),
        });
        ds.setResource(0, camera.ubo);
        ds.setResource(1, image);

        pushConstant.create(vk::ShaderStageFlagBits::eVertex, 0, sizeof(WorldPushConstant));
        pipeline.create(&vulkan.render,vulkan.device,
            projectBaseDir/"bin"/"shaders"/"chunk.spv",
            "vertMain","fragMain",
            vulkan.swapchain, dsLayout,vulkan.depthBuffer,true,
            &pushConstant
        );

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