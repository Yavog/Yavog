#pragma once
#include "text/DumpText.hpp"
#include "data/Filebasic.hpp"
#include "glm/ext/vector_int2.hpp"
#include "vulkan/Vulkan.hpp"
#include "vulkan/draw/Buffer.hpp"
#include "vulkan/draw/Descriptor.hpp"
#include "vulkan/draw/DescriptorLayout.hpp"
#include "vulkan/draw/Pipeline.hpp"
#include "vulkan/draw/PushContant.hpp"
#include "vulkan/setup/CommandBuffer.hpp"
#include "vulkan_old/Image.hpp"
#include <cstddef>
#include <filesystem>
#include <memory>


const std::vector<Vertex> vertices = {
    {{0,   0,0.f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{1.f, 0,0.f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{1.f, 0,1.f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{0,   0,1.f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};
const std::vector<uint16_t> indices = {
    0, 3, 2, 2, 1, 0};

struct GuiAsset{
    Pipeline pipelineDefault, pipelineText;
    struct ConstantBlock{
        glm::vec2 pos;
        glm::vec2 size;
    };
    struct ConstantBlockText{
        glm::vec4 color;
        glm::vec2 pos;
        glm::vec2 size;
        glm::ivec2 texSize;

        ConstantBlockText(glm::vec2 screenSize,glm::vec2 position,glm::vec2 size,Font& font,glm::vec4 color){
            this->pos     = position/screenSize;
            this->size    = size    /screenSize;
            this->texSize = font.texturePacker.getSize();
            this->color   = color;
        }
    };
    DescriptorSetLayout dsl;
    PushConstant constantDefault, constantText;
    
    Font font;
    std::shared_ptr<Image> image = std::make_shared<Image>();
    
    Buffer rectangleVB;
    Buffer rectangleIB;
    DescriptorSet dsDefault,dsText;

    
    void create(Vulkan& vulkan,std::filesystem::path projectDir){
        constantDefault.create(vk::ShaderStageFlagBits::eVertex, 0, sizeof(ConstantBlock));
        constantText.create(vk::ShaderStageFlagBits::eVertex, 0, sizeof(ConstantBlockText));

        dsl.create(vulkan.device, 
        {
            DescriptorLayout(0,vk::ShaderStageFlagBits::eFragment,vk::DescriptorType::eCombinedImageSampler)
        });

        pipelineDefault.create(
            &vulkan.render,
            vulkan.device,
            projectDir/"bin"/"shaders"/"gui.spv",
            "vertMain",
            "fragMain",
            vulkan.swapchain,
            dsl,
            vulkan.depthBuffer,false,
            &constantDefault);
        pipelineText.create(
            &vulkan.render,
            vulkan.device,
            projectDir/"bin"/"shaders"/"text.spv",
            "vertMain",
            "fragMain",
            vulkan.swapchain,
            dsl,
            vulkan.depthBuffer,false,
            &constantText);
        
        font.loadFromFile(projectDir/"assets"/"fonts"/"unscii-16-full.ttf");
        font.getGlyph(&vulkan.render,vulkan.commandPool,'-');
        dsText.create(vulkan.device, &vulkan.render, dsl, {
            DescriptorLayout(0,vk::ShaderStageFlagBits::eFragment,vk::DescriptorType::eCombinedImageSampler)
        });
        dsText.setResource(0, font.image);
        
        image->create(&vulkan.render, vulkan.commandPool, projectDir/"assets"/"texture.jpg");
        rectangleVB.createAndUpload(&vulkan.render, vulkan.commandPool, vertices.data(), vertices.size()*sizeof(Vertex), vk::BufferUsageFlagBits::eVertexBuffer);
        rectangleIB.createAndUpload(&vulkan.render, vulkan.commandPool, indices.data(), indices.size()*sizeof(uint16_t), vk::BufferUsageFlagBits::eIndexBuffer);
        dsDefault.create(vulkan.device, &vulkan.render, dsl, {
            DescriptorLayout(0,vk::ShaderStageFlagBits::eFragment,vk::DescriptorType::eCombinedImageSampler)
        });
        dsDefault.setResource(0, image);
        
    }
};