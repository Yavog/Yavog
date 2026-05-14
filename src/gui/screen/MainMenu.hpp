#pragma once
#include "client/FPSMessurement.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "gui/GuiComponent.hpp"
#include "gui/GuiScreen.hpp"
#include "gui/GuiSystem.hpp"
#include "vulkan/Vulkan.hpp"
#include <chrono>

//color = glm::vec4(0.3,0.3,0.7,1); TODO: make editable text blue

class MainMenu:public GuiScreen{
public:
    FPSMessurement fpsCounter;

    // Button button;
    // Button button2;
    TextGui   textSinglePlayer;
    TextGui   textMultiPlayer;
    TextGui   textExit;
    TextGui   textFps;

    std::chrono::steady_clock::time_point start;

    TextGui* selectedText = nullptr;
    TextGui*  hoveredText = nullptr;
    
    MainMenu(GuiSystem& guiSystem):
        GuiScreen(guiSystem),textSinglePlayer(guiSystem),
        textMultiPlayer(guiSystem),textExit(guiSystem),
        textFps(guiSystem){

    }

    glm::vec4 colorDefault = glm::vec4(0.5,0.5,0.5,1);
    glm::vec4 colorHovered = glm::vec4(1);


    void create()override{
        textSinglePlayer.setString(u8"Suffer alone    😈");
        textMultiPlayer .setString(u8"Suffer together 😈 😈");
        textExit        .setString(u8"Escape");
        textFps         .setString(u8"Hz");
        
        start = std::chrono::steady_clock::now();


        textSinglePlayer.setSize(60);
        textMultiPlayer .setSize(60);
        textExit        .setSize(60);
        textFps         .setSize(30);

        textSinglePlayer.position = glm::vec2(660,400);
        textMultiPlayer .position = glm::vec2(660,500);
        textExit        .position = glm::vec2(660,600);
        textFps         .position = glm::vec2(0,0);

        textSinglePlayer.color = colorDefault;
        textMultiPlayer .color = colorDefault;
        textExit        .color = colorDefault;
        textFps         .color = colorDefault;
    }
    void draw(CommandBuffer& CB, glm::vec2 mouse)override{    
        fpsCounter.update();
        std::string fpsString = std::to_string(fpsCounter.currentFPS)+" Hz";
        textFps   .setString(std::u8string(fpsString.begin(),fpsString.end()).c_str());


        textSinglePlayer.draw(CB,virtualScreenSize);
        textMultiPlayer .draw(CB,virtualScreenSize);
        textFps         .draw(CB,virtualScreenSize);
        if(std::chrono::steady_clock::now() < start+std::chrono::milliseconds(400))
            textExit    .draw(CB,virtualScreenSize);

        TextGui* texts[] = {&textSinglePlayer,&textMultiPlayer,&textExit};
        
        TextGui* hovered = nullptr;
        for (auto text : texts) {
            if(text->isHover(mouse)){
                hovered = text;
            }
        }
        if(hovered != hoveredText){
            if(hoveredText == &textSinglePlayer)
                textSinglePlayer.setString(u8"Suffer alone    😈");
            if(hoveredText == &textMultiPlayer)
                textMultiPlayer .setString(u8"Suffer together 😈 😈");
            if(hoveredText){
                hoveredText->color = colorDefault;
            }

            hoveredText = hovered;
            if(hoveredText)
                hoveredText->color = colorHovered;
        
            if(hoveredText       == &textSinglePlayer){
                textSinglePlayer.setString(u8"Singleplayer");
            }else if(hoveredText == &textMultiPlayer){
                textMultiPlayer .setString(u8"Multiplayer");
            }
        }
        

        if(glfwGetMouseButton(gs.vulkan.window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
            if(hoveredText == &textSinglePlayer){
                gs.setScreen(nullptr);
                return;
            }
        }
    }
    virtual ~MainMenu(){
        
    }
    bool receive(const Event& event)override{
        bool received = false;
        
        if(selectedText)
            received =  received || selectedText->receive(event);
        

        return received;
    }


};