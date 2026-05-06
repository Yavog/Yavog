#pragma once
#include <fstream>
#include <filesystem>
#include "vulkan/window/RenderSync.hpp"
#include "vulkan/window/Swapchain.hpp"
#include "vulkan/window/DepthBuffer.hpp"
#include "vulkan/draw/Buffer.hpp"
#include "vulkan_old/UBO.hpp"


//TODO move this.
struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static vk::VertexInputBindingDescription getBindingDescription()
    {
        return {.binding = 0, .stride = sizeof(Vertex), .inputRate = vk::VertexInputRate::eVertex};
    }
    static std::array<vk::VertexInputAttributeDescription,3> getAttributeDescriptions()
    {
      return {{{.location = 0, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(Vertex, pos)},
               {.location = 1, .binding = 0, .format = vk::Format::eR32G32B32Sfloat, .offset = offsetof(Vertex, color)},
               {.location = 2, .binding = 0, .format = vk::Format::eR32G32Sfloat, .offset = offsetof(Vertex, texCoord)}}};
    }
};

class Pipeline
{
    [[nodiscard]] vk::raii::ShaderModule createShaderModule(
        Device& device,
        const std::vector<char>& code) const;

    RenderSync* render = nullptr;
public:
    vk::raii::PipelineLayout pipelineLayout = nullptr;
    vk::raii::Pipeline graphicsPipeline = nullptr;

       
    void create(
        RenderSync* render,
        Device& device,
        std::filesystem::path shaderFile, 
        std::string entryFnVertex, 
        std::string entryFnFragment, 
        Swapchain& swapChain,
        class DescriptorSetLayout& dsLayout,
        DepthBuffer& depthBuffer, bool depthTesting = true,
        std::optional<class PushConstant*> pushConstant = {});
    ~Pipeline();
};

