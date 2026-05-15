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
    struct MouseButton{
        int button;
        int action;
        int mods;
    };

    class Window& window;
    std::variant<FramebufferSize,Key,Char,MouseButton> value;
    
};

struct InputHandler{
    ///@return stopPropagation
    virtual bool receive(const Event& event)=0;
};