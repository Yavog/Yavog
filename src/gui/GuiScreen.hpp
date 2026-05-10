#pragma once
#include "GLFW/glfw3.h"
#include "Text/DumpText.hpp"
#include "glm/ext/vector_float4.hpp"
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
    TextGui   text2;
    TextGui* selectedText = nullptr;
    
    Test(Vulkan& vulkan,GuiAsset& assets):GuiScreen(vulkan,assets),button(vulkan,assets),button2(vulkan,assets),text(vulkan,assets),text2(vulkan,assets){}

    void create()override{
        text.text.setString(assets.font, vulkan.commandPool, &vulkan.render, u8"Hello there");
        text.setSize(40);
        text.position.y = 0;
        text2.text.setString(assets.font, vulkan.commandPool, &vulkan.render, u8"ip adress");
        text2.setSize(40);
        text2.position.y = 50;
    }
    void draw(CommandBuffer& CB, glm::vec2 mouse)override{
        //if(!button.isHover(mouse))
        button.draw( CB);
        button2.position.x = 800;
        //if(!button2.isHover(mouse))
        button2.draw(CB);
        text.position.x = (double)vulkan.swapchain.swapChainExtent.width/2-text.getSize().x/2;
        text.draw(CB);
        text2.position.x = (double)vulkan.swapchain.swapChainExtent.width/2-text2.getSize().x/2;
        text2.draw(CB);

        if(glfwGetMouseButton(vulkan.window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
            if(selectedText){
                selectedText->color = glm::vec4(0.2,0.2,0.7,1);
            }
                
            if(text.isHover(mouse)){
                selectedText = &text;
                selectedText->color = glm::vec4(0.5,0.5,1,1);
            }
            else if(text2.isHover(mouse)){
                selectedText = &text2;
                selectedText->color = glm::vec4(0.5,0.5,1,1);
            }
            else {
                selectedText = nullptr;
            }
        }
    }
    virtual ~Test(){
        
    }
    bool receive(const Event& event)override{
        bool received = false;
        
        if(selectedText)
            received =  received || selectedText->receive(event);
        

        return received;
    }


};