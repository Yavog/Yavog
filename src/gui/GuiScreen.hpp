#pragma once
#include "Text/DumpText.hpp"
#include "gui/GuiAsset.hpp"
#include "gui/GuiComponent.hpp"
#include "vulkan/setup/CommandBuffer.hpp"
#include "vulkan/window/Event.hpp"
#include <string>
#include <variant>

class GuiScreen:public InputHandler{
protected:
    Vulkan& vulkan;
    GuiAsset& assets;
public:
    GuiScreen(Vulkan& vulkan,GuiAsset& assets):vulkan(vulkan),assets(assets){}

    virtual void create()=0;
    virtual ~GuiScreen()=default;
    virtual void draw(CommandBuffer& commandBuffer, glm::vec2 mousePosition)=0;
    virtual bool receive(const Event& event)=0;
};

class Test:public GuiScreen{
public:
    Button button;
    Button button2;
    TextGui   text;
    
    Test(Vulkan& vulkan,GuiAsset& assets):GuiScreen(vulkan,assets),button(vulkan,assets),button2(vulkan,assets),text(vulkan,assets){}

    void create()override{
        text.text.setString(assets.font, vulkan.commandPool, &vulkan.render, u8"Hello there");
        text.setSize(40);
        text.position.y = 0;
    }
    void draw(CommandBuffer& CB, glm::vec2 mouse)override{
        //if(!button.isHover(mouse))
        button.draw( CB);
        button2.position.x = 800;
        //if(!button2.isHover(mouse))
        button2.draw(CB);
        //if(!text.isHover(mouse))
        text.draw(CB);
    }
    virtual ~Test(){
        
    }
    bool receive(const Event& event)override{
        bool received = false;
        received =  received || text.receive(event);

        return received;
    }


};