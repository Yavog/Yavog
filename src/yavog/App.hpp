#pragma once
#include <filesystem>
#include <memory>
#include "glm/ext/vector_float3.hpp"
#include "yavog/client/FPSMessurement.hpp"
#include "yavog/entity/EntityManager.hpp"
#include "yavog/entity/SparseSet.hpp"
#include "yavog/gui/GuiComponent.hpp"
#include "yavog/gui/GuiScreen.hpp"
#include "yavog/gui/GuiSystem.hpp"
#include "yavog/network/connection/Client.hpp"
#include "yavog/network/connection/ServerNetworking.hpp"
#include "yavog/vulkan/Vulkan.hpp"
#include "yavog/world/ChunkDrawer.hpp"
#include "yavog/world/World.hpp"

class App{
    std::filesystem::path projectDir;
public:
    Vulkan vulkan;
    FPSMessurement fpsCounter;
    std::shared_ptr<GuiSystem> guiSystem = std::make_shared<GuiSystem>(vulkan);

    App(std::filesystem::path projectDir);
    ~App();

    ///@return restart?
    bool run();

    ServerNetworking server;
    ClientNetworking client;

    //TMP
    std::shared_ptr<class Model> model;
    struct Movement{
        glm::vec3 position;
        float phi;
        float theta;   
    };
    
    EntityManagerServer  entityManagerServer;

    SparseSet<Movement> entityMovement; 
    
    ServerWorld serverWorld;
    
    //TODO: tmp
    ChunkDrawer chunkDrawer;
    std::shared_ptr<class Chunk> chunk;

    static App* app;
};