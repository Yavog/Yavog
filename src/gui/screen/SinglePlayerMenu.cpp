#include "SinglePlayerMenu.hpp"
#include "MainMenu.hpp"
#include "GLFW/glfw3.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float4.hpp"
#include "gui/GuiScreen.hpp"
#include "gui/screen/MainMenu.hpp"
#include <memory>

SinglePlayerMenu::SinglePlayerMenu(GuiSystem& guiSystem):
    GuiScreen(guiSystem),underConstruction(gs),back(gs){

}
SinglePlayerMenu::~SinglePlayerMenu(){

}

void SinglePlayerMenu::create(){
    underConstruction.setString(u8"Singleplayer is under construction!");
    back             .setString(u8"Back");
    
    underConstruction.setSize(60);
    back             .setSize(60);

    underConstruction.position = glm::vec2(500,400);
    back             .position = glm::vec2(900,500);

    underConstruction.color = glm::vec4(1,1,1,1);
    back             .color = glm::vec4(0.5,0.5,0.5,1);
}
void SinglePlayerMenu::draw(CommandBuffer& CB, glm::vec2 mouse){
    underConstruction.draw(CB, virtualScreenSize);
    back             .draw(CB, virtualScreenSize);

    back.color = back.isHover(mouse)?glm::vec4(1):glm::vec4(0.5,0.5,0.5,1);
    if(glfwGetMouseButton(gs.vulkan.window, GLFW_MOUSE_BUTTON_1)==GLFW_PRESS)
        if(back.isHover(mouse))
            return gs.setScreen(std::make_shared<MainMenu>(gs));
}
bool SinglePlayerMenu::receive(const Event& event){
    return false;
}