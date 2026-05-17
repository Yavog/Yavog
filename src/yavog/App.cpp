#include <cassert>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include "yavog/App.hpp"
#include "FastNoiseLite.h"
#include <GLFW/glfw3.h>
#include <numbers>
#include <ratio>
#include <thread>
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "yavog/gui/screen/MainMenu.hpp"
#include "yavog/vulkan/draw/PushContant.hpp"
#include "yavog/world/Chunk.hpp"
#include "yavog/vulkan/model/Model.hpp"
#include "yavog/world/World.hpp"

App* App::app = nullptr;

void generateChunk(Chunk& chunk){
    FastNoiseLite noise;
    noise.SetSeed(rand());
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
    

    model = std::make_shared<Model>();
    model->create( projectDir/"assets"/"model"/"Human.glb",projectDir,world.camera);
    
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

        if( !client.update() && !guiSystem->screen ){
            guiSystem->setScreen(std::make_shared<MainMenu>(*guiSystem));
            chunk.reset();
            chunk = std::make_shared<Chunk>();
            generateChunk(*chunk);
        }
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
            if(chunk){
                
                world.pushConstant.use(CB, world.pipeline, World::WorldPushConstant{.position = glm::vec3(0)});
                chunk->draw(CB);
            }

            //camera 
            if(vulkan.window.isMouseGrabbed())
                world.camera.update(vulkan.window,fpsCounter.delta);  
            
            for(auto& position:entityPositions)
            {
                glm::mat4 matrix(1);
                matrix = glm::translate(matrix, position-glm::vec3(0,2,0));
                matrix = glm::rotate(matrix, (float)(phi+std::numbers::pi) , glm::vec3(0.0f, 1.0f, 0.0f));
                model->pushConstant.use(CB, model->pipeline, Model::ModelPushConstant{
                    .matrix = matrix
                });
                model->draw(CB);
            }
            //if(!client.cnc.con->isClose)
            client.pl.playerMovement.sendServer(client.cnc.con->toServer, world.camera);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        guiSystem->draw(CB);


        vulkan.swapchain.endRendering(CB, imageIndex);
        CB.end();
        vulkan.render.end(vulkan.window, vulkan.swapchain, vulkan.commandPool, &vulkan.depthBuffer, imageIndex);

    }
    return false;
}