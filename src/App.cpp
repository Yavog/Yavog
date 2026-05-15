#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include "App.hpp"
#include "FastNoiseLite.h"
#include "GLFW/glfw3.h"
#include "world/Chunk.hpp"

App* App::app = nullptr;

void generateChunk(Chunk& chunk){
    time_t t;
    time(&t);
    FastNoiseLite noise;
    noise.SetSeed(t);
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFrequency(0.005);
    //noise.SetFractalGain(0.75);
    noise.SetFractalOctaves(5);
    //noise.SetFractalLacunarity(2);    

    for (int x = 0; x < Chunk::chunkSize; x++) {
        for (int y = 0; y < Chunk::chunkSize; y++) {
            for (int z = 0; z < Chunk::chunkSize; z++) {
                chunk.chunkData[x][y][z].type = noise.GetNoise((float)x, (float)y, (float)z)>0;
            }
        }
    }
}

App::App(std::filesystem::path projectDir):projectDir(projectDir){
    assert(!app);
    app = this;


    time_t t;
    time(&t);
    FastNoiseLite noise;
    noise.SetSeed(t);
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFrequency(0.005);
    //noise.SetFractalGain(0.75);
    noise.SetFractalOctaves(5);
    //noise.SetFractalLacunarity(2);    

    chunk = std::make_shared<Chunk>();
    generateChunk(*chunk);


}
App::~App(){
    assert(app);
    app = nullptr;
}


bool App::run(){    
    std::cout<<projectDir<<std::endl;
    
    guiSystem->create(projectDir);
    vulkan.window.inputHandler = guiSystem;

    world.init(vulkan, projectDir);
    

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
        
        if(glfwGetKey(vulkan.window, GLFW_KEY_P) == GLFW_PRESS)
        {
            generateChunk(*chunk);
            for(auto& player:server.players)
                server.procotolList.chunk.send(player.con->toClient, *App::app->chunk);
        }

        //TODO: TMP
        {
            world.draw(vulkan,CB, imageIndex);
            if(chunk)
                chunk->draw(CB);
            //camera 
            if(vulkan.window.isMouseGrabbed())
                world.camera.update(vulkan.window,fpsCounter.delta);  
        }

        vulkan.swapchain.endRendering(CB, imageIndex);
        CB.end();
        vulkan.render.end(vulkan.window, vulkan.swapchain, vulkan.commandPool, &vulkan.depthBuffer, imageIndex);

    }
    return false;
}