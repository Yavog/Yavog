#include "MultiplayerJoinMenu.hpp"
#include "App.hpp"
#include "glm/ext/vector_float2.hpp"
#include "gui/screen/MainMenu.hpp"
#include "gui/screen/MultiplayerMenu.hpp"
#include "network/connection/Client.hpp"
#include "network/connection/Server.hpp"
#include <cstddef>
#include <exception>
#include <string>

MultiplayerJoinMenu::MultiplayerJoinMenu(GuiSystem& guiSystem)
    :GuiScreen(guiSystem),username(gs),serverIp(gs),join(gs),back(gs){

}
MultiplayerJoinMenu::~MultiplayerJoinMenu(){
}

void MultiplayerJoinMenu::create(){
    username.defaultText = u8"Enter name of sacrifice";
    serverIp.defaultText = u8"Enter Server Ip";

    back    .setString(u8"Back");
    join    .setString(u8"Join");
    username.setString(u8"");
    serverIp.setString(u8"");

    back    .setSize(60);
    join    .setSize(60);
    username.setSize(60);
    serverIp.setSize(60);

    back    .position = glm::vec2(0,0);
    join    .position = glm::vec2(1200,900);
    username.position = glm::vec2(300,420);
    serverIp.position = glm::vec2(300,300);

    back    .color = colorDefault;
    join    .color = colorDefault;
    username.color = colorEditable;
    serverIp.color = colorEditable;
}
void MultiplayerJoinMenu::draw(CommandBuffer& CB, glm::vec2 mouse){
    back    .draw(CB,virtualScreenSize);
    join    .draw(CB,virtualScreenSize);
    username.draw(CB,virtualScreenSize);
    serverIp.draw(CB,virtualScreenSize);

    TextGui* texts[] = {&username,&serverIp,&join,&back};
    TextGui* hovered = nullptr;
    for (auto text : texts) {
        if(text->isHover(mouse)){
            hovered = text;
        }
    }
    if(hovered != hoveredText){
        if(hoveredText){
            if(hoveredText == &back || hoveredText == &join)
                hoveredText->color = colorDefault;
            if(hoveredText == &username || hoveredText == &serverIp)
                if(hoveredText!=selectedText)
                    hoveredText->color = colorEditable;
        }
        hoveredText = hovered;
        if(hoveredText){
            if(hoveredText == &back || hoveredText == &join)
                hoveredText->color = colorHovered;
            if(hoveredText == &username || hoveredText == &serverIp)
                if(hoveredText!=selectedText)
                    hoveredText->color = colorEditableHovered;
        }
    }
}
bool MultiplayerJoinMenu::receive(const Event& event){
    bool received = false;

    if(selectedText && selectedText->receive(event))
        return true;
    
    if(std::holds_alternative<Event::MouseButton>(event.value)){
        auto ev = std::get<Event::MouseButton>(event.value);
        if(ev.button == GLFW_MOUSE_BUTTON_1 && ev.action == GLFW_PRESS){
            if(hoveredText == &back){
                gs.setScreen(std::make_shared<MultiplayerMenu>(gs));
                return true;
            }else if(hoveredText == &join){
                if(username.text.string.size() == 0){
                    username.color = colorInvalid;
                    return true;
                }
                if(serverIp.text.string.size() == 0){
                    serverIp.color = colorInvalid;
                    return true;
                }

                Client& client = App::app->client;

                
                if(!client.join(serverIp.text.string)){
                    join.setString(u8"couldn't connect. retry...");
                    join.text.string.clear();
                    serverIp.color = colorInvalid;
                    return true;
                }

                gs.setScreen(nullptr);
                return true;
            }else if(selectedText != hoveredText){
                //old
                if(selectedText){
                    selectedText->color = colorEditable;
                }
                selectedText = hoveredText;
                //new
                if(selectedText){
                    selectedText->color = colorSelected;
                    return true;
                }
                else return false;
            }
        }
    }
    return received;
}
