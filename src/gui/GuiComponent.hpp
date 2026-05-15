#pragma once
#include "GLFW/glfw3.h"
#include "text/DumpText.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float4.hpp"
#include "gui/GuiAsset.hpp"
#include "gui/GuiSystem.hpp"
#include "vulkan/Vulkan.hpp"
#include "vulkan/setup/CommandBuffer.hpp"
#include "vulkan/setup/CommandPool.hpp"
#include <string>
#include <string_view>

class GuiComponent:public InputHandler{
protected:
    Vulkan& vulkan;
    GuiAsset& assets;
    glm::vec2 size     = glm::vec2(600,400);
public:
    glm::vec2 position = glm::vec2(100,100);
    GuiComponent(Vulkan& vulkan,GuiAsset& assets):vulkan(vulkan),assets(assets){}
    virtual void draw(CommandBuffer& cb,glm::vec2 screenSize)=0;
    bool isHover(glm::vec2 mousePosition){
        return (position.x <= mousePosition.x && mousePosition.x <= position.x + size.x && position.y <= mousePosition.y && mousePosition.y <= position.y + size.y);
    }
    glm::vec2 getSize()const{
        return size;
    }
};


class Button:public GuiComponent{
public:
    Button(Vulkan &vulkan, GuiAsset &assets):GuiComponent(vulkan,assets){}
    virtual void draw(CommandBuffer& cb,glm::vec2 screenSize)override{
        assets.dsDefault.bind(vulkan.device, cb.commandBuffer, vulkan.render, assets.pipelineDefault);
        assets.constantDefault.use(cb, assets.pipelineDefault, GuiAsset::ConstantBlock{
            .pos =  position/screenSize,
            .size = size/screenSize
        });
        cb.commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, assets.pipelineDefault.graphicsPipeline);
        cb.commandBuffer.bindVertexBuffers(0, *assets.rectangleVB.buffer, {0});
        cb.commandBuffer.bindIndexBuffer(*assets.rectangleIB.buffer, 0, vk::IndexType::eUint16);
        cb.commandBuffer.drawIndexed(6 /*TODO: indexBuffer->count*/, 1, 0, 0, 0);
    }
    virtual bool receive(const Event& event)override{
        return false;
    }
};

struct TextGui:public GuiComponent{
    std::u8string defaultText;
public:
    glm::vec4 color = glm::vec4(1);
    Text text;

    TextGui(GuiSystem& gs):GuiComponent(gs.vulkan,gs.assets){}
    virtual void draw(CommandBuffer& cb,glm::vec2 screenSize)override{
        assets.dsText.bind(vulkan.device,cb.commandBuffer,vulkan.render,assets.pipelineText);
        cb.commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *assets.pipelineText.graphicsPipeline);
        
        assets.constantText.use(cb,assets.pipelineText,GuiAsset::ConstantBlockText(screenSize,position,glm::vec2(size.y),assets.font,color));
        
        size.x = size.y*text.width;
        text.draw(cb);
    }

    void setSize(double y){
        size.y = y;
    }

    void setString(std::string_view str){
        std::u8string_view u8Str(reinterpret_cast<const char8_t*>(str.data()),str.size());
        setString(u8Str);
    }
    void setString(std::u8string_view str){
        if(str.size())
            text.setString(assets.font, vulkan.commandPool, &vulkan.render, str);
        else {
            text.setString(assets.font, vulkan.commandPool, &vulkan.render, defaultText);
            text.string.clear();
        }
    }
    void setString(std::u32string str){
        if(str.size())
            text.setString(assets.font, vulkan.commandPool, &vulkan.render, str);
        else{
            text.setString(assets.font, vulkan.commandPool, &vulkan.render, defaultText);
            text.string.clear();
        }
    }
    
    virtual bool receive(const Event& event)override{
        if(std::holds_alternative<Event::Char>(event.value)){
            auto ev = std::get<Event::Char>(event.value);
            text.setString(assets.font, vulkan.commandPool, &vulkan.render, 
                text.string+(char32_t)ev.codepoint
            );
            return true;
        }
        if(std::holds_alternative<Event::Key>(event.value)){
            auto ev = std::get<Event::Key>(event.value);
            if(ev.action == GLFW_PRESS || ev.action == GLFW_REPEAT){
                if(ev.key == GLFW_KEY_BACKSPACE){
                    if(text.string.size()){
                        setString(text.string.substr(0,text.string.size()-1));
                    }
                }
            }
            
            return true;
        }

        return false;
    }

};