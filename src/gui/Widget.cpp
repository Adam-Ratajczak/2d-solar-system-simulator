#include "Widget.hpp"

#include "Application.hpp"
#include "Container.hpp"

#include <cassert>
#include <iostream>
#include <typeinfo>

Widget::Widget(Container& parent)
    : m_parent(&parent)
    , m_application(parent.m_application) {
}

Widget::~Widget() {
    if (m_application.focused_widget() == this)
        m_application.set_focused_widget(nullptr);
}

bool Widget::is_mouse_over(sf::Vector2i mouse_pos) const {
    return sf::Rect<float>(m_pos, m_size).contains(mouse_pos.x, mouse_pos.y);
}

void Widget::do_handle_event(Event& event) {
    Widget::handle_event(event);
    handle_event(event);
}

void Widget::do_update() {
    update();
}

void Widget::set_focused() {
    m_application.set_focused_widget(this);
}

bool Widget::is_focused() const {
    return m_application.focused_widget() == this;
}

void Widget::handle_event(Event& event) {
    if (event.type() == sf::Event::MouseMoved) {
        sf::Vector2i mouse_pos { event.event().mouseMove.x, event.event().mouseMove.y };
        m_hover = is_mouse_over(mouse_pos);
    }
    else if (event.type() == sf::Event::MouseButtonPressed) {
        if (m_hover) {
            set_focused();
            event.set_handled();
        }
    }
    else if (event.type() == sf::Event::Resized)
        set_needs_relayout();
}

void Widget::draw(sf::RenderWindow& window) const {
    sf::RectangleShape outline(size());
    outline.setPosition(position());
    outline.setOutlineThickness(1);
    outline.setOutlineColor(sf::Color::Red);
    outline.setFillColor(sf::Color::Transparent);
    window.draw(outline);
}

void Widget::relayout_and_draw(sf::RenderWindow& window) {
    relayout_if_needed();
    Widget::draw(window);
    this->draw(window);
}

void Widget::relayout_if_needed() {
    if (!m_needs_relayout)
        return;
    // std::cout << this << ":" << typeid(*this).name() << m_size.x << "," << m_size.y << "@" << m_pos.x << "," << m_pos.y << std::endl;
    this->relayout();
    m_needs_relayout = false;
}

sf::RenderWindow& Widget::window() const {
    return m_application.window();
}

void Widget::dump(unsigned depth) {
    for (int i = 0; i < depth; i++)
        std::cout << "-   ";
    std::cout << typeid(*this).name() << ": pos=" << m_pos.x << "," << m_pos.y << " size=" << m_size.x << "," << m_size.y << std::endl;
}
