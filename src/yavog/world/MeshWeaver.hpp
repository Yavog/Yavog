#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_int3.hpp"
#include "yavog/vulkan/draw/Pipeline.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <vector>

class MeshWeaver
{
    const glm::ivec3 LUTneighbours[3] = {
        glm::ivec3(1,0,0),
        glm::ivec3(0,1,0),
        glm::ivec3(0,0,1),
    };
    const glm::vec3 LUTfaces[3][4] = {
        {
            glm::vec3(1,0,0),
            glm::vec3(1,1,0),
            glm::vec3(1,1,1),
            glm::vec3(1,0,1),
        },
        {
            glm::vec3(0,1,0),
            glm::vec3(0,1,1),
            glm::vec3(1,1,1),
            glm::vec3(1,1,0),
        },
        {
            glm::vec3(1,0,1),
            glm::vec3(1,1,1),
            glm::vec3(0,1,1),
            glm::vec3(0,0,1),
        },
    };
    
    const uint8_t orderPositive[6] = {0, 1, 2, 2, 3, 0};
    const uint8_t orderNegatvie[6] = {0, 3, 2, 2, 1, 0};
public:    
    static constexpr int chunkSize = 31;
    Vertex vertices[32768];
    uint16_t index[32768];

    size_t vIndex = 0;
    size_t iIndex = 0;
    void create(class Chunk& chunk);
};