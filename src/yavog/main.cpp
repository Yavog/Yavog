#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <iostream>
#include "yavog/App.hpp"
#include "yavog/data/BinaryData.hpp"
#include "yavog/vulkan/model/Model.hpp"
#ifdef _WIN32
    #include <windows.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"


int experimentServer();
int experimentParser();




int main(int argc, char const *argv[]){        
    
    setlocale(LC_ALL, "en_US.utf8");
    
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    // return experimentServer();
    // return experimentParser();
    
    time_t t;
    time(&t);
    srand(t);
    
    auto projectBaseDir = std::filesystem::canonical(argv[0]).parent_path().parent_path().parent_path();
    try{
        while(true){
            App app(projectBaseDir);
            if(!app.run())
                return 0;
            // reload for different mods.
        }
    } catch (const vk::SystemError& err){
        std::cerr << "Vulkan error: " << err.what() << std::endl;
        return 1;
    }catch (const std::exception& err){
        std::cerr << "Error: " << err.what() << std::endl;
        return 1;
    }
    return 0;
}