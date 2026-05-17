#pragma once

#include <cassert>
#include <cstdint>
#include <optional>
#include <vector>
template<class T>
class SparseSet{
    std::vector<T> dense;
    std::vector<uint32_t> sparseToDense;
    std::vector<uint32_t> denseToSparse;
public:
    std::optional<T*> get(uint32_t id){
        if(sparseToDense.size() <= id  || sparseToDense[id] == -1){
            return std::nullopt;
        }
        assert(dense.size() > sparseToDense[id]);
        return &dense[sparseToDense[id]];
    }
    void insert(uint32_t id){
        if(sparseToDense.size() <= id){
            sparseToDense.resize(id+1, -1);   
        }
        if(sparseToDense[id] == -1){
            sparseToDense[id] = dense.size();
            dense.emplace_back();
            denseToSparse.push_back(id);
        }
    }
    void erase(uint32_t id){
        if(sparseToDense.size() <= id || sparseToDense[id] == -1){
            return;   
        }
        // swap remove
        auto index = sparseToDense[id];
        std::swap(dense[index],dense[dense.size()-1]);
        std::swap(denseToSparse[index],denseToSparse[dense.size()-1]);
        sparseToDense[denseToSparse[index]] = index;

        dense.pop_back();
        denseToSparse.pop_back();
        
        sparseToDense[id] = -1;
    }
    void clear(){
        dense.clear();
        sparseToDense.clear();
        denseToSparse.clear();
    }
    
    auto begin()       {return dense.begin();}    
    auto end()         {return dense.end  ();}
    auto begin() const {return dense.begin();}    
    auto end()   const {return dense.end  ();}
    
    

};