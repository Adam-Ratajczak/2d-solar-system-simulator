#include "Widget.hpp"

bool Widget::is_mouse_over(sf::Vector2i mouse_pos) const
{
    return sf::Rect<float>(m_pos, m_size).contains(mouse_pos.x, mouse_pos.y);
}

void Widget::handle_event(sf::Event& event)
{
    if(event.type == sf::Event::MouseMoved)
    {
        sf::Vector2i mouse_pos { event.mouseMove.x, event.mouseMove.y };
        m_hover = is_mouse_over(mouse_pos);
    }
}

void Widget::relayout_if_needed()
{
    if(!m_needs_relayout)
        return;
    m_needs_relayout = true;

    // TODO: Actually implement layout.
}
