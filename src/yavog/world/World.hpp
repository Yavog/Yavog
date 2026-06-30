#pragma once

#include "glm/ext/vector_int3.hpp"
#include <map>
#include <memory>

class ServerWorld{
    std::map<glm::ivec3, std::shared_ptr<class Chunk>> chunks;
public:
    

    ServerWorld(){

    }
    ~ServerWorld(){

    }
};