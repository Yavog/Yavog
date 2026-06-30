#include <algorithm>
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
#include "glm/ext/vector_int3.hpp"
#include "glm/geometric.hpp"
#include "yavog/client/Camera.hpp"
#include "yavog/gui/screen/MainMenu.hpp"
#include "yavog/vulkan/draw/PushContant.hpp"
#include "yavog/vulkan/window/Window.hpp"
#include "yavog/world/Chunk.hpp"
#include "yavog/vulkan/model/Model.hpp"
#include "yavog/world/ChunkDrawer.hpp"

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

std::chrono::time_point<std::chrono::steady_clock> tmp; //tmp
std::chrono::time_point<std::chrono::steady_clock> lastPlace(std::chrono::milliseconds(0)),lastBreak(std::chrono::milliseconds(0)); //tmp


bool App::run(){    
    std::cout<<projectDir<<std::endl;
    
    guiSystem->create(projectDir);
    vulkan.window.inputHandler = guiSystem;

    chunkDrawer.init(vulkan, projectDir);
    

    model = std::make_shared<Model>();
    model->create( projectDir/"assets"/"model"/"Human.glb",projectDir,chunkDrawer.camera);
    
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
            chunkDrawer.draw(vulkan,CB, imageIndex);
            if(chunk){
                
                chunkDrawer.pushConstant.use(CB, chunkDrawer.pipeline, ChunkDrawer::WorldPushConstant{.position = glm::vec3(0)});
                chunk->draw(CB);
                
                //camera 
                chunkDrawer.camera.velocity  -= glm::vec3(0.f,1.f,0.f)*9.81f*fpsCounter.delta;
            }
            if(vulkan.window.isMouseGrabbed())
                chunkDrawer.camera.update(vulkan.window,fpsCounter.delta);  

            if(glfwGetKey(vulkan.window, GLFW_KEY_F)){
                chunkDrawer.camera.velocity.y = 0;
                chunkDrawer.camera.pos.y = 32;
            }
            //std::cout << world.camera.velocity.y << std::endl;
            for (int k = 0; k<3; k++) {
            
                auto tColl = chunk->collision(chunkDrawer.camera.pos-glm::vec3(0.40,1.75,0.40),glm::vec3(0.8,1.8,0.8),chunkDrawer.camera.velocity);
                auto t = std::max({tColl[0],tColl[1],tColl[2]});
                //std::cout <<t <<"\t"<< tColl[0] << "\t" << tColl[1] << "\t" << tColl[2] << std::endl;
                
                if(t <= fpsCounter.delta*2){
                    
                    int i = 0;
                    t = 0;
                    for (int j = 0; j<3; j++) {
                        if(tColl[j]>t){
                            t = tColl[j];
                            i = j;
                        }
                    }
                    chunkDrawer.camera.velocity[i] *= -0.1;
                    
                }else break;
            }
            chunkDrawer.camera.pos += chunkDrawer.camera.velocity*fpsCounter.delta;

            auto ray = chunk->ray(chunkDrawer.camera.pos, chunkDrawer.camera.forward);
            if(ray.available)
            {
                //std::cout << ray.breakPosition[0] <<"\t"<< ray.breakPosition[1] <<"\t"<< ray.breakPosition[2] <<"\t"<<std::endl;
                std::cout << ray.placePosition[0] <<"\t"<< ray.placePosition[1] <<"\t"<< ray.placePosition[2] <<"\t"<<std::endl;
                if(glfwGetMouseButton(vulkan.window, GLFW_MOUSE_BUTTON_LEFT) && lastBreak + std::chrono::milliseconds(200)< std::chrono::steady_clock::now()){
                    lastBreak = std::chrono::steady_clock::now();
                    
                    chunk->chunkData[ray.breakPosition[0]][ray.breakPosition[1]][ray.breakPosition[2]].type = 0;
                    chunk->create();
                }
                else if(glfwGetMouseButton(vulkan.window, GLFW_MOUSE_BUTTON_RIGHT) && lastPlace + std::chrono::milliseconds(200)< std::chrono::steady_clock::now()){
                    lastPlace = std::chrono::steady_clock::now();
                    
                    auto tv = chunk->aabb({
                        .start = ray.placePosition,
                        .end = ray.placePosition+glm::ivec3(1,1,1),
                    }, {
                        .start = chunkDrawer.camera.pos-glm::vec3(0.40,1.75,0.40),
                        .end   = chunkDrawer.camera.pos-glm::vec3(0.40,1.75,0.40)+glm::vec3(0.8,1.8,0.8)
                    }, chunkDrawer.camera.velocity);
                    auto t = std::max({tv[0],tv[1],tv[2]});
                    if(t > fpsCounter.delta*2){
                        chunk->chunkData[ray.placePosition[0]][ray.placePosition[1]][ray.placePosition[2]].type = 1;
                        chunk->create();
                    }
                }
            }
           
            for(auto& movement:entityMovement)
            {
                glm::mat4 matrix(1);
                matrix = glm::translate(matrix, movement.position-glm::vec3(0,2,0));
                matrix = glm::rotate(matrix, (float)(movement.phi+std::numbers::pi) , glm::vec3(0.0f, 1.0f, 0.0f));
                model->pushConstant.use(CB, model->pipeline, Model::ModelPushConstant{
                    .matrix = matrix
                });
                model->draw(CB);
            }
            //if(!client.cnc.con->isClose)
            if(tmp+std::chrono::milliseconds(10) < std::chrono::steady_clock::now()){
                tmp = std::chrono::steady_clock::now();
                client.pl.playerMovement.sendServer(client.cnc.con->toServer, chunkDrawer.camera);
            }
        }
        
        

        guiSystem->draw(CB);


        vulkan.swapchain.endRendering(CB, imageIndex);
        CB.end();
        vulkan.render.end(vulkan.window, vulkan.swapchain, vulkan.commandPool, &vulkan.depthBuffer, imageIndex);

    }
    return false;
}