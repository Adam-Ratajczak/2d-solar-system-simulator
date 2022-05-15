#pragma once

#include "ToolWindow.hpp"

namespace GUI {

class MessageBox : public ToolWindow {
public:
    enum class Buttons {
        YesNo,
        Ok
    };

    explicit MessageBox(sf::RenderWindow& wnd, sf::String message, sf::String title, Buttons buttons);

    enum class ButtonRole {
        Yes,
        No,
        Ok
    };

    ButtonRole exec() {
        run();
        return m_clicked_button;
    }

private:
    ButtonRole m_clicked_button {};
};

}
