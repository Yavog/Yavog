#pragma once


#include "glm/ext/vector_float3.hpp"
#include "yavog/App.hpp"
#include "yavog/data/BinaryData.hpp"
#include "yavog/vulkan/draw/Buffer.hpp"
#include "yavog/vulkan/draw/Pipeline.hpp"
#include "yavog/world/MeshWeaver.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
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
        MeshWeaver& mw = MeshWeaver::mw;
        RenderSync* render = &App::app->vulkan.render;
        Device& device = App::app->vulkan.device;
        CommandPool& pool = App::app->vulkan.commandPool;
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
            assert(mw.vIndex < 11901677568);
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
    struct Box{
        glm::vec3 start;
        glm::vec3 end;
    };
    glm::vec3 aabb(const Box& a,const Box& b,glm::vec3 bVelocity){
        glm::vec3 t;
        for (int i = 0; i < 3 ; i++) {
            if(bVelocity[i] > 0){
                t[i] = (a.start[i]-b.end[i])/bVelocity[i];
            }else if(bVelocity[i] < 0){
                t[i] = (b.start[i]-a.end[i])/abs(bVelocity[i]);
            }else {
                if(a.start[i] < b.end[i] && b.start[i] < a.end[i])
                    t[i] = 0;
                else
                    t[i] = std::numeric_limits<float>::max();
            }
            if(t[i] < 0){
                if(a.start[i] < b.end[i] && b.start[i] < a.end[i])
                    t[i] = 0;
                else
                    t[i] = std::numeric_limits<float>::max();
            }
        }
        return t;
    }
    glm::vec3 collision(glm::vec3 position,glm::vec3 size,glm::vec3 velocity){
        auto end = position+size;
        glm::vec3 tNextCollVec = glm::vec3(std::numeric_limits<float>::max());
        float tNextColl = std::numeric_limits<float>::max();
        for (int x = std::floor(position.x-1); x <= std::ceil(end.x); x++) {
            for (int y = std::floor(position.y-1); y <= std::ceil(end.y); y++) {
                for (int z = std::floor(position.z-1); z <= std::ceil(end.z); z++) {
                    if(0 <= x && x < chunkSize && 0 <= y && y < chunkSize && 0 <= z && z < chunkSize){
                        auto block = chunkData[x][y][z];
                        if(block.type == 1){
                            auto tCollVec = aabb({.start = glm::vec3(x,y,z),.end = glm::vec3(x+1,y+1,z+1)},
                             {.start = position,.end = end}, velocity);
                            auto tColl = std::max({tCollVec[0], tCollVec[1],tCollVec[2]});
                            if( tNextColl > tColl){
                               tNextColl = tColl;
                               tNextCollVec = tCollVec;
                            }
                        }
                    }
                }
            }
        }
        return tNextCollVec;
    }
};