#pragma once


#include "yavog/App.hpp"
#include "yavog/data/BinaryData.hpp"
#include "yavog/vulkan/draw/Buffer.hpp"
#include "yavog/vulkan/draw/Pipeline.hpp"
#include "yavog/world/MeshWeaver.hpp"
#include <cstddef>
#include <cstdint>
#include <iostream>
typedef uint16_t BlockType;
typedef uint16_t BlockData;
struct Block{
    BlockType type;
    BlockData data;
};
class Chunk{
public:
    static const uint32_t chunkSize = 32;
    Block chunkData[chunkSize][chunkSize][chunkSize]; 
    Chunk();
    Chunk(BinaryData& bd){
        for (int x = 0; x<chunkSize; x++) {
            for (int y = 0; y<chunkSize; y++) {
                for (int z = 0; z<chunkSize; z++) {
                    if(!bd.readU16(chunkData[x][y][z].type) 
                    || !bd.readU16(chunkData[x][y][z].data)){
                        std::cout << "corrupted chunkData" << std::endl;
                        return;
                    }
                }
            }    
        }

        create();
    }

    operator BinaryData(){
        BinaryData rv;
        for (int x = 0; x<chunkSize; x++) {
            for (int y = 0; y<chunkSize; y++) {
                for (int z = 0; z<chunkSize; z++) {
                    rv.writeU16(chunkData[x][y][z].type);
                    rv.writeU16(chunkData[x][y][z].data);
                }
            }    
        }
        return rv;
    } 


    //TODO refactor this:
    Buffer vertexBuffer,indexBuffer;
    // std::vector<Vertex> vertices;
    // std::vector<uint16_t> indices;
    size_t indices = 0;

    void create(){
        RenderSync* render = &App::app->vulkan.render;
        Device& device = App::app->vulkan.device;
        CommandPool& pool = App::app->vulkan.commandPool;
        MeshWeaver mw;
        {
            char* data = new char[33*33*33];
            for (size_t x = 0; x < 33; x++){
                for (size_t y = 0; y < 33; y++){
                    for (size_t z = 0; z < 33; z++){
                        int value = 0;
                        if(x==0||y==0||z==0||x==32||y==32||z==32)
                            value = 0;
                        else value = chunkData[x][y][z].type;
                        data[x*33*33+y*33+z] = value;
                    }
                }
            }
            
            auto t1 = std::chrono::steady_clock::now();
            //mw.create(data,0,0,0);
            mw.create(*this);
            auto t2 =  std::chrono::steady_clock::now();
            //std::cout <<"mesh generation time:"<< std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count()<<"µs" <<std::endl;
        }
        //vertices = *(std::vector<Vertex>*)&mw.vertices; // me being a bad boy. Because i am lazy.
        indices = mw.iIndex;
        // vertices.resize(mw.vIndex);
        // indices .resize(mw.iIndex);

        if(indices){
            vertexBuffer.createAndUpload( render,pool,mw.vertices,mw.vIndex*sizeof(Vertex),vk::BufferUsageFlagBits::eVertexBuffer);
            indexBuffer.createAndUpload(  render,pool,mw.index,mw.iIndex *sizeof(uint16_t),vk::BufferUsageFlagBits::eIndexBuffer);
        }
    }
    void draw(CommandBuffer& buffer){
        if(indices){
            auto& commandBuffer = buffer.commandBuffer;
            commandBuffer.bindVertexBuffers(0, *vertexBuffer.buffer, {0});
            commandBuffer.bindIndexBuffer(*indexBuffer.buffer, 0, vk::IndexType::eUint16);
            commandBuffer.drawIndexed(static_cast<uint32_t>(indices), 1, 0, 0,0);
        }
    }
    
};