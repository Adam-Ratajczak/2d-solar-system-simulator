#pragma once

#include "WidgetTreeRoot.hpp"

namespace GUI {

class ToolWindow : public WidgetTreeRoot {
public:
    ToolWindow(sf::RenderWindow& wnd)
        : WidgetTreeRoot(wnd) { }

    virtual sf::Vector2f position() const override { return m_position; }
    void set_position(sf::Vector2f position) { m_position = position; }

    virtual sf::Vector2f size() const override { return m_size; }
    void set_size(sf::Vector2f size) { m_size = size; }

private:
    sf::Vector2f m_position;
    sf::Vector2f m_size;
};

}
