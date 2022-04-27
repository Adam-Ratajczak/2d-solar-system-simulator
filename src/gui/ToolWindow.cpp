#include "ToolWindow.hpp"
#include "Widget.hpp"

#include <iostream>

namespace GUI {

void ToolWindow::handle_event(Event& event) {
    if (event.type() == sf::Event::MouseButtonPressed) {
        sf::Vector2f mouse_position { static_cast<float>(event.event().mouseButton.x), static_cast<float>(event.event().mouseButton.y) };
        mouse_position += position();
        if (titlebar_rect().contains(mouse_position)) {
            m_dragging = true;
            m_drag_position = mouse_position;
            return;
        }
    }
    else if (event.type() == sf::Event::MouseMoved) {
        sf::Vector2f mouse_position { static_cast<float>(event.event().mouseMove.x), static_cast<float>(event.event().mouseMove.y) };
        mouse_position += position();
        if (m_dragging) {
            auto delta = mouse_position - m_drag_position;
            std::cout << delta.x << "," << delta.y << std::endl;
            m_position += delta;
            m_drag_position = mouse_position;
            return;
        }
    }
    else if (event.type() == sf::Event::MouseButtonReleased) {
        m_dragging = false;
    }

    WidgetTreeRoot::handle_event(event);
}

}
