#pragma once
#include <filesystem>
#include "vulkan/Vulkan.hpp"

class App{
    std::filesystem::path projectDir;
public:
    Vulkan vulkan;


    App(std::filesystem::path projectDir);
    
    ///@return restart?
    bool run();
};