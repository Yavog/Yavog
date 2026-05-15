#include "MultiplayerHostMenu.hpp"
#include "App.hpp"
#include "glm/ext/vector_float2.hpp"
#include "gui/screen/MainMenu.hpp"
#include "gui/screen/MultiplayerMenu.hpp"
#include "network/connection/Client.hpp"
#include "network/connection/Server.hpp"
#include <cstddef>
#include <exception>
#include <string>

MultiplayerHostMenu::MultiplayerHostMenu(GuiSystem& guiSystem)
    :GuiScreen(guiSystem),username(gs),port(gs),host(gs),back(gs){

}
MultiplayerHostMenu::~MultiplayerHostMenu(){
}

void MultiplayerHostMenu::create(){
    username.defaultText = u8"Enter name of sacrifice";
    port.defaultText = u8"Enter Port";

    back    .setString(u8"Back");
    host    .setString(u8"Host");
    username.setString(u8"");
    port.setString(u8"");

    back    .setSize(60);
    host    .setSize(60);
    username.setSize(60);
    port.setSize(60);

    back    .position = glm::vec2(0,0);
    host    .position = glm::vec2(1200,900);
    username.position = glm::vec2(300,420);
    port.position = glm::vec2(300,300);

    back    .color = colorDefault;
    host    .color = colorDefault;
    username.color = colorEditable;
    port.color = colorEditable;
}
void MultiplayerHostMenu::draw(CommandBuffer& CB, glm::vec2 mouse){
    back    .draw(CB,virtualScreenSize);
    host    .draw(CB,virtualScreenSize);
    username.draw(CB,virtualScreenSize);
    port.draw(CB,virtualScreenSize);

    TextGui* texts[] = {&username,&port,&host,&back};
    TextGui* hovered = nullptr;
    for (auto text : texts) {
        if(text->isHover(mouse)){
            hovered = text;
        }
    }
    if(hovered != hoveredText){
        if(hoveredText){
            if(hoveredText == &back || hoveredText == &host)
                hoveredText->color = colorDefault;
            if(hoveredText == &username || hoveredText == &port)
                if(hoveredText!=selectedText)
                    hoveredText->color = colorEditable;
        }
        hoveredText = hovered;
        if(hoveredText){
            if(hoveredText == &back || hoveredText == &host)
                hoveredText->color = colorHovered;
            if(hoveredText == &username || hoveredText == &port)
                if(hoveredText!=selectedText)
                    hoveredText->color = colorEditableHovered;
        }
    }
}
bool MultiplayerHostMenu::receive(const Event& event){
    bool received = false;

    if(selectedText && selectedText->receive(event))
        return true;
    
    if(std::holds_alternative<Event::MouseButton>(event.value)){
        auto ev = std::get<Event::MouseButton>(event.value);
        if(ev.button == GLFW_MOUSE_BUTTON_1 && ev.action == GLFW_PRESS){
            if(hoveredText == &back){
                gs.setScreen(std::make_shared<MultiplayerMenu>(gs));
                return true;
            }else if(hoveredText == &host){
                if(username.text.string.size() == 0){
                    username.color = colorInvalid;
                    return true;
                }
                if(port.text.string.size() == 0){
                    port.color = colorInvalid;
                    return true;
                }

                auto u8port = ClientNetworkConnection::toUTF8(port.text.string);
                std::string portStr(u8port.begin(),u8port.end());
                size_t portNumber;
                try {
                    portNumber = std::stoi(portStr);
                } catch (const std::exception& e) {
                    port.setString(u8"invalid port");
                    port.text.string.clear();
                    port.color = colorInvalid;
                    return true;
                }

                Server& server = App::app->server;
                if(!server.listen(portNumber)){
                    port.setString(u8"couldn't use this port.");
                    port.text.string.clear();
                    port.color = colorInvalid;
                    return true;
                }
                // while (true) {
                //     server.update();
                // }
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
