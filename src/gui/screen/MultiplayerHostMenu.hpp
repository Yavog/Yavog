#include "gui/GuiComponent.hpp"
#include "gui/GuiScreen.hpp"

class MultiplayerHostMenu:public GuiScreen{
    TextGui back;

    TextGui username;
    TextGui port;
    TextGui host;

    glm::vec4 colorDefault         = glm::vec4(0.5,0.5,0.5,1);
    glm::vec4 colorHovered         = glm::vec4(1);
    glm::vec4 colorEditable        = glm::vec4(0.3,0.3,0.7,1);
    glm::vec4 colorEditableHovered = glm::vec4(0.2,0.2,0.7,1);
    glm::vec4 colorSelected        = glm::vec4(0,0.5,1,1);
    glm::vec4 colorInvalid        = glm::vec4(1,0.1,0.1,1);
    

    TextGui* hoveredText = nullptr,* selectedText = nullptr;
public:
    MultiplayerHostMenu(GuiSystem& guiSystem);
    virtual ~MultiplayerHostMenu();

    void create()override;
    void draw(CommandBuffer& CB, glm::vec2 mouse)override;
    bool receive(const Event& event)override;
};