#pragma once

#include "ToolWindow.hpp"

namespace GUI {

// FIXME: This should be a modal dialog.
class MessageBox : public ToolWindow {
public:
    enum class Buttons
    {
        YesNo,
        Ok
    };

    explicit MessageBox(sf::RenderWindow& wnd, sf::String message, sf::String title, Buttons buttons);

    enum class ButtonRole
    {
        Yes,
        No,
        Ok
    };

    std::function<void(ButtonRole)> on_finish;
};

}
