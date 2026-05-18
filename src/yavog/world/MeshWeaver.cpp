#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "yavog/gui/GuiAsset.hpp"
#include "yavog/world/Chunk.hpp"
#include <atomic>

void MeshWeaver::create(class Chunk& chunk){
    auto t1 = std::chrono::steady_clock::now();
    vIndex = 0;
    iIndex = 0;
    for ( int x = 0; x < chunkSize; x++) {
        for ( int y = 0; y < chunkSize; y++) {
            for ( int z = 0; z < chunkSize; z++) {
                Block block = chunk.chunkData[x][y][z];
                for (int j = 0; j<3 ; j++) {
                    const auto n = LUTneighbours[j]; 
                    const Block block2 = chunk.chunkData[x+n.x][y+n.y][z+n.z];
                    
                    if(block.type != block2.type){
                        const bool lookFromPositive = block2.type==0;
                        const size_t verticeIndex = vIndex;

                        glm::vec3 pos(x,y,z);
                        glm::vec3 color = pos;
                        color/=32;
                        auto w = glm::vec2(x,y)/32.f;

                        glm::vec2 uvBase;
                        glm::vec2 textureAtlasSize(3,1);
                        if(j == 1){
                            if(lookFromPositive){
                                uvBase = glm::vec2(0,0);
                            }
                            else{
                                uvBase = glm::vec2(2,0);
                            }
                        }else
                            uvBase = glm::vec2(1,0);

                        vertices[vIndex++] = {
                            .pos = pos+LUTfaces[j][0],
                            .color = color,
                            .texCoord = (uvBase+glm::vec2(1,1))/textureAtlasSize,
                        };
                        vertices[vIndex++] = {
                            .pos = pos+LUTfaces[j][1],
                            .color = color,
                            .texCoord = (uvBase+glm::vec2(1,0))/textureAtlasSize,
                        };
                        vertices[vIndex++] = {
                            .pos = pos+LUTfaces[j][2],
                            .color = color,
                            .texCoord = (uvBase+glm::vec2(0,0))/textureAtlasSize,
                        };
                        vertices[vIndex++] = {
                            .pos = pos+LUTfaces[j][3],
                            .color = color,
                            .texCoord = (uvBase+glm::vec2(0,1))/textureAtlasSize,
                        };
                        if(lookFromPositive){
                            for (auto &&i : orderPositive){
                                index[iIndex++] = (verticeIndex+i);
                            }
                        }else{
                            for (auto &&i : orderNegatvie){
                                index[iIndex++] = (verticeIndex+i);
                            }
                        }
                    }
                }

            }
        }
    } 
    auto t2 = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
    std::cout <<"mesh generation: "<< ms <<"µs"<<std::endl;
}