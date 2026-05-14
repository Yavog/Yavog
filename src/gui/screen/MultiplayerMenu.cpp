#include "gui/screen/MultiplayerMenu.hpp"
#include "gui/screen/MainMenu.hpp"
#include "gui/screen/SinglePlayerMenu.hpp"
#include "gui/screen/MultiplayerHostMenu.hpp"

MultiplayerMenu::MultiplayerMenu(GuiSystem& guiSystem):
    GuiScreen(guiSystem),textHost(guiSystem),
    textJoin(guiSystem),back(guiSystem){

}

void MultiplayerMenu::create(){
    textHost.setString(u8"host a game");
    textJoin.setString(u8"join a game");
    back    .setString(u8"back");


    textHost.setSize(60);
    textJoin .setSize(60);
    back            .setSize(60);
    
    textHost.position = glm::vec2(660,400);
    textJoin.position = glm::vec2(660,500);
    back    .position = glm::vec2(0,0);
    
    textHost.color = colorDefault;
    textJoin.color = colorDefault;
    back    .color = colorDefault;
}
void MultiplayerMenu::draw(CommandBuffer& CB, glm::vec2 mouse){    
    textHost.draw(CB,virtualScreenSize);
    textJoin.draw(CB,virtualScreenSize);
    back    .draw(CB,virtualScreenSize);

    TextGui* texts[] = {&textHost,&textJoin,&back};
    
    TextGui* hovered = nullptr;
    for (auto text : texts) {
        if(text->isHover(mouse)){
            hovered = text;
        }
    }

    if(hovered != hoveredText){
        if(hoveredText)
            hoveredText->color = colorDefault;
        hoveredText = hovered;
        if(hoveredText)
            hoveredText->color = colorHovered;
    }
}
MultiplayerMenu::~MultiplayerMenu(){
    
}
bool MultiplayerMenu::receive(const Event& event){
    bool received = false;
    
    if(selectedText && selectedText->receive(event))
        return true;

    if(std::holds_alternative<Event::MouseButton>(event.value)){
        auto ev = std::get<Event::MouseButton>(event.value);
        if(ev.button == GLFW_MOUSE_BUTTON_1 && ev.action == GLFW_PRESS){
            if(hoveredText == &textHost){
                gs.setScreen(std::make_shared<MultiplayerHostMenu>(gs));
                return true;
            }else if(hoveredText == &textJoin){
                gs.setScreen(std::make_shared<SinglePlayerMenu>(gs));
                return true;
            }else if(hoveredText == &back){
                gs.setScreen(std::make_shared<MainMenu>(gs));
                return true;
            }           
        }
    }
 
    return received;
}
