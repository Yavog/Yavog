#pragma once
#include "glm/ext/vector_float2.hpp"
#include "vulkan/Vulkan.hpp"
#include "vulkan/setup/CommandBuffer.hpp"

class GuiComponent{
    glm::vec2 position;
    
    virtual void draw(Vulkan& vulkan,CommandBuffer& cb)=0;
};