#pragma once
#include <filesystem>
#include <memory>
#include "glm/ext/vector_float3.hpp"
#include "yavog/client/FPSMessurement.hpp"
#include "yavog/gui/GuiComponent.hpp"
#include "yavog/gui/GuiScreen.hpp"
#include "yavog/gui/GuiSystem.hpp"
#include "yavog/network/connection/Client.hpp"
#include "yavog/network/connection/Server.hpp"
#include "yavog/vulkan/Vulkan.hpp"
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

    std::shared_ptr<class Model> model;
    glm::vec3 position = glm::vec3(0);

    Server server;
    Client client;

    //TODO: tmp
    World world;
    std::shared_ptr<class Chunk> chunk;

    static App* app;
};