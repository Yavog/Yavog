#pragma once
#include <cassert>
#include <filesystem>
#include <iostream>
#include "cgltf.h"

class Model{
public:
    void processNode(cgltf_node& node){
        std::cout << node.name << std::endl;   
        for (int i = 0; i < node.children_count; i++) {
            processNode(*node.children[i]);
        }
        for (int i = 0; i < node.mesh->primitives_count; i++) {
            auto p = node.mesh->primitives[i];
            
            for (int j = 0; j < p.attributes_count; j++) {
                auto attr = p.attributes[j];
                if(attr.type == cgltf_attribute_type_position){
                    auto accessor = attr.data;
                    assert(accessor->type == cgltf_type_vec3);
                    std::cout << accessor->count <<" ";     

                    for (int v = 0; v < accessor->count; v++) {
                        cgltf_float position[3];
                        assert(cgltf_accessor_read_float(accessor, v,  position, 3));
                        std::cout << position[0] <<"\t"<< position[1] <<"\t"<< position[2] <<" "<<std::endl;     
                    }
                } 
            }
        }
        
    }

    void create(std::filesystem::path fspath){
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
    }
};