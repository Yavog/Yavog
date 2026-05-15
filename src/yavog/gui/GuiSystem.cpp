#include "gui/GuiSystem.hpp"
#include "gui/screen/MainMenu.hpp"
#include "gui/GuiScreen.hpp"

GuiSystem::GuiSystem(Vulkan& vulkan):vulkan(vulkan){

}

void GuiSystem::setScreen(std::shared_ptr<GuiScreen> newScreen){
    if(newScreen){
        newScreen->create();
    }
    screen = newScreen;
}
void GuiSystem::create(std::filesystem::path projectDir){
    assets.create(vulkan,projectDir);
    setScreen(std::make_shared<MainMenu>(*this));
}
void GuiSystem::draw(CommandBuffer& CB){
    glm::dvec2 mouse;
    glfwGetCursorPos(vulkan.window, &mouse.x, &mouse.y);
    
    if(screen){
        glm::vec2 screenSize = glm::vec2(vulkan.swapchain.swapChainExtent.width,vulkan.swapchain.swapChainExtent.height);
        mouse *= screen->virtualScreenSize/screenSize; 
        screen->draw(CB, mouse);
    }
}
bool GuiSystem::receive(const Event& event){
    if(screen)
        return screen->receive(event);
    return false;
}