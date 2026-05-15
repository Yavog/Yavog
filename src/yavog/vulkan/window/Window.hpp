#pragma once
#include "vulkan/Header.hpp"
#include "vulkan/setup/Instance.hpp"
#include "vulkan/window/Event.hpp"
#include <memory>


class Window
{
    GLFWwindow *window = nullptr;
    
    struct{
        int xpos,ypos;
        int sizex,sizey;
    }beforeFullscreen;
    
    bool grabMouse = true;
public:
    vk::raii::SurfaceKHR surface = nullptr;
    
    Window(InstanceSettings* settings);
    Window(const Window&)=delete;
    ~Window();

    void create(Instance& instance,int width, int height, const char *title);
    void close();
    bool update();
    
    operator GLFWwindow*();
    
    void toggleFullscreen();
    void toggleMouseGrab();

    [[deprecated]] bool framebufferResized = false;
    bool isMouseGrabbed()const;

    [[deprecated]] std::u32string textInput;

    std::shared_ptr<InputHandler> inputHandler;
};
            