

#include "GLFW/glfw3.h"
#include "client/FPSMessurement.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float4.hpp"
#include "network/connection/Client.hpp"
#include "network/connection/Server.hpp"
#include "vulkan/draw/DescriptorLayout.hpp"
#include "vulkan/setup/Instance.hpp"
#include "vulkan/window/Window.hpp"
#include "vulkan/setup/Device.hpp"
#include "vulkan/setup/CommandBuffer.hpp"
#include "vulkan/window/RenderSync.hpp"
#include "vulkan/draw/Pipeline.hpp"
#include "vulkan/draw/Descriptor.hpp"
#include "client/Camera.hpp"
#include "Text/DumpText.hpp"
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cuchar>
#include <memory>
#include <string>
#include <vector>

#include "client/Game.hpp"
#include "vulkan_old/Image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "client/MeshWeaver.hpp"
#include "FastNoiseLite.h"
#include "vulkan/draw/PushContant.hpp"
#include "vulkan/draw/DescriptorLayout.hpp"


const std::vector<Vertex> vertices = {
    {{0,   0,0.f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{1.f, 0,0.f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{1.f, 0,1.f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{0,   0,1.f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};
const std::vector<uint16_t> indices = {
    0, 3, 2, 2, 1, 0};

FPSMessurement fpsCounter;

MeshWeaver mw;
struct Chunk2{
    Buffer vertexBuffer,indexBuffer;
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    void create(RenderSync* render,Device& device,CommandPool& pool,FastNoiseLite& noise,size_t xOffset,size_t yOffset,size_t zOffset){
        {
            char* data = new char[33*33*33];
            for (size_t x = 0; x < 33; x++){
                for (size_t y = 0; y < 33; y++){
                    for (size_t z = 0; z < 33; z++){
                        int value = (noise.GetNoise((float)x+xOffset, (float)z+zOffset)*32+32)>(float) y+yOffset;
                        // if(x==0||y==0||z==0||x==32||y==32||z==32)
                        // value = 0;
                        data[x*33*33+y*33+z] = value;
                    }
                }
            }
            
            auto t1 = std::chrono::steady_clock::now();
            mw.create(data,xOffset,yOffset,zOffset);
            auto t2 =  std::chrono::steady_clock::now();
            std::cout <<"mesh generation time:"<< std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count()<<"µs" <<std::endl;
        }
        vertices = *(std::vector<Vertex>*)&mw.vertices; // me being a bad boy. Because i am lazy.
        indices = mw.index;

        if(indices.size()){
            vertexBuffer.createAndUpload(render,pool,vertices.data(),vertices.size(),vk::BufferUsageFlagBits::eVertexBuffer);
            indexBuffer.createAndUpload(  render,pool,indices.data(),indices.size(),vk::BufferUsageFlagBits::eIndexBuffer);
        }
    }
    void draw(CommandBuffer& buffer){
        if(indices.size()){
            auto& commandBuffer = buffer.commandBuffer;
            commandBuffer.bindVertexBuffers(0, *vertexBuffer.buffer, {0});
            commandBuffer.bindIndexBuffer(*indexBuffer.buffer, 0, vk::IndexType::eUint16);
            commandBuffer.drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0,0);
        }
    }
};
struct WorldRenderer{
    Image image;
    DescriptorSetLayout dsLayout;
    DescriptorSet ds;
    Camera camera;
    Pipeline pipeline;
    
    FastNoiseLite noise;
    static const size_t range = 3;
    Chunk2 chunk[range][range][range];

    void init(Game2& _game,std::filesystem::path projectBaseDir,DepthBuffer& depthBuffer){

        time_t t;
        time(&t);
        noise.SetSeed(t);
        noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        noise.SetFractalType(FastNoiseLite::FractalType_FBm);
        noise.SetFrequency(0.005);
        //noise.SetFractalGain(0.75);
        noise.SetFractalOctaves(5);
        //noise.SetFractalLacunarity(2);    

        image.create(&_game.render,_game.commandPool,projectBaseDir/"assets"/"texture.jpg");
        
        camera.create(_game.device,&_game.render);
        dsLayout.create(_game.device,{
            DescriptorLayout(0,vk::ShaderStageFlagBits::eVertex  ,vk::DescriptorType::eUniformBuffer),
            //DescriptorLayout(1,vk::ShaderStageFlagBits::eFragment,vk::DescriptorType::eCombinedImageSampler),
        });
        ds.create(_game.device,&_game.render,dsLayout,{
            DescriptorLayout(0,vk::ShaderStageFlagBits::eVertex  ,vk::DescriptorType::eUniformBuffer),
            //Descriptor(1,vk::ShaderStageFlagBits::eFragment,image),
        });
        ds.setResource(0, camera.ubo);

        depthBuffer.create(&_game.render,_game.commandPool,_game.swapchain);
        pipeline.create(&_game.render,_game.device,
            projectBaseDir/"bin"/"shaders"/"slang.spv",
            "vertMain","fragMain",
            _game.swapchain, dsLayout,depthBuffer);

        for (size_t x = 0; x < range; x++){
            for (size_t y = 0; y < range; y++){
                for (size_t z = 0; z < range; z++){
                    chunk[x][y][z].create(&_game.render,_game.device,_game.commandPool,noise,x*32,-32+y*32,z*32);
                }
            }
            
        }
    }
    void draw(Game2& _game,CommandBuffer& CB,uint32_t frameIndex,uint32_t imageIndex){
        float aspectRatio = static_cast<float>(_game.swapchain.swapChainExtent.width) / static_cast<float>(_game.swapchain.swapChainExtent.height);

        bool zoom = glfwGetKey(_game.window,GLFW_KEY_C) == GLFW_PRESS;
        camera.updateUniformBuffer(frameIndex,aspectRatio, zoom,camera.pos,camera.forward);

        auto& commandBuffer = CB.commandBuffer;
        
        {
            commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline.graphicsPipeline);
            commandBuffer.setViewport(0, vk::Viewport{
                .x = 0.0f,
                .y = 0.0f,
                .width = static_cast<float>(_game.swapchain.swapChainExtent.width),
                .height = static_cast<float>(_game.swapchain.swapChainExtent.height),
                .minDepth = 0.0f,
                .maxDepth = 1.0f,
            });
            commandBuffer.setScissor(0, vk::Rect2D{
                .offset = vk::Offset2D{.x = 0,.y = 0},
                .extent = _game.swapchain.swapChainExtent,
            });
            
            //TODO: learn more about dynamic descriptors
            ds.bind(_game.device,commandBuffer,_game.render,pipeline);
    
            for (size_t x = 0; x < range; x++){
                for (size_t y = 0; y < range; y++){
                    for (size_t z = 0; z < range; z++){
                        chunk[x][y][z].draw(CB);
                    }
                }
            }
        }
    }

};
void game(Game2& _game,std::filesystem::path projectBaseDir) {
    DepthBuffer depthBuffer;
    WorldRenderer wr;
    wr.init(_game, projectBaseDir,depthBuffer);
    
    // TODO refactor the following in the future:
    auto recordCommandBuffer = [&](CommandBuffer& CB,uint32_t frameIndex,uint32_t imageIndex)
    {
        CB.begin();
        _game.swapchain.beginRendering(CB,imageIndex,&depthBuffer);
        
        wr.draw(_game,CB, frameIndex, imageIndex);
            
        _game.swapchain.endRendering(CB,imageIndex);
        CB.end();
    };

    while(_game.window.update()){
        glfwPollEvents();
        //drawing
        {
            ImageIndex imageIndex;
            if(!_game.render.begin(_game.window,_game.swapchain,_game.commandPool,&depthBuffer,imageIndex))
                continue;

            auto frameIndex = _game.render.getFrameIndex();
            auto& CB        = _game.render.getCommandBuffer();
            
            recordCommandBuffer(CB,frameIndex,imageIndex);
            _game.render.end(_game.window,_game.swapchain,_game.commandPool,&depthBuffer,imageIndex);
        }
        
        //FRAMERATE STUFF
        fpsCounter.update();

        //camera 
        if(_game.window.isMouseGrabbed())
        {
            wr.camera.update(_game.window,fpsCounter.delta);   
        }
        
    }
    _game.device.device.waitIdle();
}

