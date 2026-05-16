#pragma once
#include <cassert>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <memory>
#include <vector>
#include "cgltf.h"
#include "glm/ext/vector_float2.hpp"
#include "yavog/App.hpp"
#include "yavog/vulkan/draw/Buffer.hpp"
#include "yavog/vulkan/draw/Descriptor.hpp"
#include "yavog/vulkan/draw/DescriptorLayout.hpp"
#include "yavog/vulkan/draw/Pipeline.hpp"
#include "yavog/vulkan_old/Image.hpp"

class Model{

    Buffer vertexBuffer,indexBuffer;
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
public:
    std::shared_ptr<Image> image = std::make_shared<Image>();
    void processNode(cgltf_node& node){
        std::cout << node.name << std::endl;   
        for (int i = 0; i < node.children_count; i++) {
            processNode(*node.children[i]);
        }
        for (int i = 0; i < node.mesh->primitives_count; i++) {
            auto p = node.mesh->primitives[i];
            assert(p.type == cgltf_primitive_type_triangles);

            for (int j = 0; j < p.attributes_count; j++) {
                auto attr = p.attributes[j];
                auto accessor = attr.data;
                
                if(attr.type == cgltf_attribute_type_position){
                    assert(accessor->type == cgltf_type_vec3);

                    for (int v = 0; v < accessor->count; v++) {
                        cgltf_float position[3];
                        assert(cgltf_accessor_read_float(accessor, v,  position, 3));
                        vertices.push_back(Vertex{
                            .pos = glm::vec3(position[0],position[1],position[2]),
                            .color = glm::vec3(1,1,1),
                            .texCoord = glm::vec2(1,1)
                        });  
                    }
                }
                if(attr.type == cgltf_attribute_type_texcoord){
                    assert(accessor->type == cgltf_type_vec2);
                    assert(accessor->count == vertices.size());
                    for (int v = 0; v < accessor->count ; v++) {
                        cgltf_float uv[2];
                        assert(cgltf_accessor_read_float(accessor, v,  uv, 2));
                        std::cout << uv[0]<< " "<<uv[1]<<std::endl;
                        vertices[v].color.x = uv[0];
                        vertices[v].color.y = uv[1];

                        vertices[v].texCoord = glm::vec2(uv[0],uv[1]); 
                    }
                }
            }
            for (int j = 0; j < p.indices->count; j++) {
                auto id = cgltf_accessor_read_index(p.indices, j);
                indices.push_back(id);
            }           
            auto material = p.material;
            assert(material->has_pbr_metallic_roughness);
            {
                auto tex = material->pbr_metallic_roughness.base_color_texture.texture;
                if(tex && tex->image){
                    auto image = tex->image;
                    if(image->uri){
                        std::string uri(image->uri);
                        std::cout <<uri<<std::endl;
                        assert(0 && "unimplemented");
                    }
                    else if(image->buffer_view){
                        auto view = image->buffer_view;
                        auto data = (const uint8_t*)view->buffer->data + view->offset;
                        auto size = view->size;
                        auto mime_type = image->mime_type;

                        std::string name(image->name);
                        int width  = 0;
                        int height = 0;
                        int channels_in_file = 0;
                        auto pxls = stbi_load_from_memory(data, size,&width,&height, &channels_in_file, 4);
                        if(pxls){
                            this->image->create(&App::app->vulkan.render, App::app->vulkan.commandPool, width, height, pxls);
                            stbi_image_free(pxls);
                        }
                        std::cout <<mime_type<<std::endl;
                    }
                }
            }
        }
        
    }
    DescriptorSetLayout dsl;
    DescriptorSet ds;
    Pipeline pipeline;
    PushConstant pushConstant;

    Model(){

    }

    void create(std::filesystem::path fspath,std::filesystem::path projectDir,Camera& camera){
        std::string path = fspath;
        cgltf_options options = {};
        cgltf_data* data = NULL;

        cgltf_result result = cgltf_parse_file(&options, path.c_str(), &data);
        if (result == cgltf_result_success)
        {
            result = cgltf_load_buffers(&options,data,path.c_str());

            for (int i= 0; i< data->nodes_count; i++) {
                std::cout << data->nodes[i].name << std::endl;   
                processNode(data->nodes[0]);         
            }

            cgltf_free(data);
        }

        RenderSync* render = &App::app->vulkan.render;
        Device& device = App::app->vulkan.device;
        CommandPool& pool = App::app->vulkan.commandPool;

        if(indices.size()){
            vertexBuffer.createAndUpload(render,pool,vertices.data(),vertices.size()*sizeof(Vertex),vk::BufferUsageFlagBits::eVertexBuffer);
            indexBuffer.createAndUpload(  render,pool,indices.data(),indices.size() *sizeof(uint16_t),vk::BufferUsageFlagBits::eIndexBuffer);
        }

        // pipeline

        dsl.create(App::app->vulkan.device,{
            DescriptorLayout(0,vk::ShaderStageFlagBits::eVertex  ,vk::DescriptorType::eUniformBuffer),
            DescriptorLayout(1,vk::ShaderStageFlagBits::eFragment,vk::DescriptorType::eCombinedImageSampler),
        });
        ds.create(App::app->vulkan.device,&App::app->vulkan.render,dsl,{
            DescriptorLayout(0,vk::ShaderStageFlagBits::eVertex  ,vk::DescriptorType::eUniformBuffer),
            DescriptorLayout(1,vk::ShaderStageFlagBits::eFragment,vk::DescriptorType::eCombinedImageSampler),
        });
        ds.setResource(0, camera.ubo);
        ds.setResource(1, image);

        pushConstant.create(vk::ShaderStageFlagBits::eVertex, 0, sizeof(World::WorldPushConstant));

        pipeline.create(&App::app->vulkan.render,App::app->vulkan.device,
            projectDir/"bin"/"shaders"/"model.spv",
            "vertMain","fragMain",
            App::app->vulkan.swapchain, dsl,App::app->vulkan.depthBuffer,true,
            &pushConstant
        );
    }
    void draw(CommandBuffer& buffer){
        buffer.commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline.graphicsPipeline);
        ds.bind(App::app->vulkan.device,buffer.commandBuffer,App::app->vulkan.render,pipeline);

        if(indices.size()){
            auto& commandBuffer = buffer.commandBuffer;
            commandBuffer.bindVertexBuffers(0, *vertexBuffer.buffer, {0});
            commandBuffer.bindIndexBuffer(*indexBuffer.buffer, 0, vk::IndexType::eUint16);
            commandBuffer.drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0,0);
        }
    }
};