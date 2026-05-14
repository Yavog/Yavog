#pragma once
#include "GLFW/glfw3.h"
#include "Text/DumpText.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float4.hpp"
#include "gui/GuiAsset.hpp"
#include "gui/GuiComponent.hpp"
#include "gui/GuiSystem.hpp"
#include "vulkan/setup/CommandBuffer.hpp"
#include "vulkan/window/Event.hpp"
#include <string>
#include <variant>

class GuiScreen:public InputHandler{
protected:
    GuiSystem& gs;
public:
    glm::vec2 virtualScreenSize = glm::vec2(1920,1080);
    GuiScreen(GuiSystem& guiSystem):gs(guiSystem){}
    virtual ~GuiScreen()=default;

    virtual void create()=0;
    virtual void draw(CommandBuffer& commandBuffer, glm::vec2 mousePosition)=0;
    virtual bool receive(const Event& event)=0;
};

class Test:public GuiScreen{
public:
    // Button button;
    // Button button2;
    TextGui   text;
    TextGui   text2;
    TextGui* selectedText = nullptr;
    
    Test(GuiSystem& guiSystem):GuiScreen(guiSystem),text(guiSystem),text2(guiSystem){}

    void create()override{
        text.defaultText = u8"Enter your name here!";
        text.setString(u8"");
        text.setSize(40);
        text.position.y = 0;
        text2.defaultText = u8"Enter Server address here!";
        text2.setString(u8"");
        text2.setSize(40);
        text2.position.y = 50;

        text. color = glm::vec4(0.2,0.2,0.7,1);
        text2.color = glm::vec4(0.2,0.2,0.7,1);

    }
    void draw(CommandBuffer& CB, glm::vec2 mouse)override{
        //if(!button.isHover(mouse))
        // button.draw( CB);
        // button2.position.x = 800;
        // //if(!button2.isHover(mouse))
        // button2.draw(CB);

        
        text.position.x = virtualScreenSize.x/2-text.getSize().x/2;
        text.draw(CB,virtualScreenSize);
        text2.position.x = virtualScreenSize.x/2-text2.getSize().x/2;
        text2.draw(CB,virtualScreenSize);


        if(glfwGetMouseButton(gs.vulkan.window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
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