#pragma once
#include "client/FPSMessurement.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "gui/GuiComponent.hpp"
#include "gui/GuiScreen.hpp"
#include "gui/GuiSystem.hpp"
#include "vulkan/Vulkan.hpp"
#include <chrono>


class MainMenu:public GuiScreen{
public:
    FPSMessurement fpsCounter;

    // Button button;
    // Button button2;
    TextGui   textSinglePlayer;
    TextGui   textMultiPlayer;
    TextGui   textExit;
    
    std::chrono::steady_clock::time_point start;

    TextGui* selectedText = nullptr;
    TextGui*  hoveredText = nullptr;

    glm::vec4 colorDefault = glm::vec4(0.5,0.5,0.5,1);
    glm::vec4 colorHovered = glm::vec4(1);

    MainMenu(GuiSystem& guiSystem);
    virtual ~MainMenu();

    void create()override;
    void draw(CommandBuffer& CB, glm::vec2 mouse)override;
    bool receive(const Event& event)override;
};