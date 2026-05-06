#pragma once

#include <variant>
struct Event{
    struct FramebufferSize{
        int width; 
        int height;
    };
    struct Key{
        int key;
        int scancode;
        int action;
        int mods;
    };
    struct Char{
        unsigned int codepoint;
    };

    class Window& window;
    std::variant<FramebufferSize,Key,Char> value;
    
};

struct InputHandler{
    ///@return stopPropagation
    virtual bool receive(const Event& event)=0;
};