#pragma once
#include "yavog/entity/SparseSet.hpp"
#include <cstddef>
#include <cstdint>
#include <set>
#include <vector>

typedef uint32_t Entity;

class EntityManagerServer{
    Entity endId = 0;
    std::vector<Entity> freeSet; 
public:
    Entity addEntity(){
        if(!freeSet.empty()){
            auto id = freeSet.back();
            freeSet.pop_back();
            return id;
        }else{
            return endId++;
        }
    }
    void removeEntity(uint32_t id){
        freeSet.push_back(id);
    }
};