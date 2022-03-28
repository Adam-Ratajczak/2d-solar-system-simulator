#include "Widget.hpp"

#include "Container.hpp"

#include <cassert>
#include <iostream>
#include <typeinfo>

Widget::Widget(Container& parent)
: m_parent(&parent), m_window(parent.m_window)
{
}

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
    else if(event.type == sf::Event::Resized)
        set_needs_relayout();
}

void Widget::draw(sf::RenderWindow& window) const
{
    sf::RectangleShape outline(size());
    outline.setPosition(position());
    outline.setOutlineThickness(1);
    outline.setOutlineColor(sf::Color::Red);
    outline.setFillColor(sf::Color::Transparent);
    window.draw(outline);
}

void Widget::update_and_draw(sf::RenderWindow& window)
{
    update();
    relayout_if_needed();
    Widget::draw(window);
    this->draw(window);
}

void Widget::relayout_if_needed()
{
    if(!m_needs_relayout)
        return;
    // std::cout << this << ":" << typeid(*this).name() << m_size.x << "," << m_size.y << "@" << m_pos.x << "," << m_pos.y << std::endl;
    this->relayout();
    m_needs_relayout = false;
}
