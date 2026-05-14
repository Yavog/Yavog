#include "gui/GuiScreen.hpp"

class SinglePlayerMenu:public GuiScreen{
public:
    TextGui   underConstruction;
    TextGui   back;



    SinglePlayerMenu(GuiSystem& guiSystem);
    virtual ~SinglePlayerMenu();

    void create()override;
    void draw(CommandBuffer& CB, glm::vec2 mouse)override;
    bool receive(const Event& event)override;
};