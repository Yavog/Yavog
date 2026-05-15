#pragma once
#include <filesystem>
#include <memory>
#include "client/FPSMessurement.hpp"
#include "gui/GuiComponent.hpp"
#include "gui/GuiScreen.hpp"
#include "gui/GuiSystem.hpp"
#include "network/connection/Client.hpp"
#include "network/connection/Server.hpp"
#include "vulkan/Vulkan.hpp"
#include "world/World.hpp"

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


    Server server;
    Client client;

    //TODO: tmp
    World world;
    std::shared_ptr<class Chunk> chunk;

    static App* app;
};