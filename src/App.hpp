#pragma once
#include <filesystem>
#include <memory>
#include "gui/GuiComponent.hpp"
#include "gui/GuiScreen.hpp"
#include "vulkan/Vulkan.hpp"

class App{
    std::filesystem::path projectDir;
public:
    Vulkan vulkan;

    std::shared_ptr<GuiScreen> screen;
    App(std::filesystem::path projectDir);
    
    ///@return restart?
    bool run();
};