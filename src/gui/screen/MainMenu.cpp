#include "gui/screen/MainMenu.hpp"
#include "gui/screen/MultiplayerMenu.hpp"
#include "gui/screen/SinglePlayerMenu.hpp"
#include "gui/screen/MultiplayerHostMenu.hpp"

MainMenu::MainMenu(GuiSystem& guiSystem):
    GuiScreen(guiSystem),textSinglePlayer(guiSystem),
    textMultiPlayer(guiSystem),textExit(guiSystem){

}

void MainMenu::create(){
    textSinglePlayer.setString(u8"Suffer alone    😈");
    textMultiPlayer .setString(u8"Suffer together 😈 😈");
    textExit        .setString(u8"Escape");
    //textFps         .setString(u8"Hz");
    
    start = std::chrono::steady_clock::now();


    textSinglePlayer.setSize(60);
    textMultiPlayer .setSize(60);
    textExit        .setSize(60);
    //textFps         .setSize(30);

    textSinglePlayer.position = glm::vec2(660,400);
    textMultiPlayer .position = glm::vec2(660,500);
    textExit        .position = glm::vec2(660,600);
    //textFps         .position = glm::vec2(0,0);

    textSinglePlayer.color = colorDefault;
    textMultiPlayer .color = colorDefault;
    textExit        .color = colorDefault;
    //textFps         .color = colorDefault;
}
void MainMenu::draw(CommandBuffer& CB, glm::vec2 mouse){    
    fpsCounter.update();
    std::string fpsString = std::to_string(fpsCounter.currentFPS)+" Hz";
    //textFps   .setString(std::u8string(fpsString.begin(),fpsString.end()).c_str());


    textSinglePlayer.draw(CB,virtualScreenSize);
    textMultiPlayer .draw(CB,virtualScreenSize);
    //textFps         .draw(CB,virtualScreenSize);
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
    
}
MainMenu::~MainMenu(){
    
}
bool MainMenu::receive(const Event& event){
    bool received = false;
    
    if(selectedText && selectedText->receive(event))
        return true;
    if(std::holds_alternative<Event::MouseButton>(event.value)){
        auto ev = std::get<Event::MouseButton>(event.value);
        if(ev.button == GLFW_MOUSE_BUTTON_1 && ev.action == GLFW_PRESS){
            if(hoveredText == &textSinglePlayer){
                gs.setScreen(std::make_shared<SinglePlayerMenu>(gs));
                return true;
            }else if(hoveredText == &textMultiPlayer){
                gs.setScreen(std::make_shared<MultiplayerMenu>(gs));
                return true;
            }
        }
    }

    return received;
}