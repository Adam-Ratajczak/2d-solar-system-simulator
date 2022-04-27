#pragma once

#include "WidgetTreeRoot.hpp"

namespace GUI {

class ToolWindow : public WidgetTreeRoot {
public:
    ToolWindow(sf::RenderWindow& wnd)
        : WidgetTreeRoot(wnd) { }

    static constexpr auto TitleBarSize = 28;

    virtual sf::Vector2f position() const override { return m_position; }
    void set_position(sf::Vector2f position) { m_position = position; }

    virtual sf::Vector2f size() const override { return m_size; }
    void set_size(sf::Vector2f size) { m_size = size; }

    void set_title(sf::String string) { m_string = std::move(string); }
    sf::String title() const { return m_string; }

    sf::FloatRect full_rect() const { return { position() - sf::Vector2f(0, TitleBarSize), size() + sf::Vector2f(0, TitleBarSize) }; }
    sf::FloatRect titlebar_rect() const { return { position() - sf::Vector2f(0, TitleBarSize), { size().x, TitleBarSize } }; }

    void handle_event(Event&);

private:
    sf::Vector2f m_position;
    sf::Vector2f m_size;
    sf::String m_string;
    bool m_dragging = false;
    sf::Vector2f m_drag_position;
};

}
