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

void Widget::update() {
    if (!m_tooltip_text.empty()) {
        // std::cout << this << ": " << m_tooltip_counter << std::endl;
        if (m_tooltip_counter > 0)
            m_tooltip_counter--;
        if (m_hover) {
            if (m_tooltip_counter == 0 && !m_tooltip) {
                // TODO: Use mouse position;
                m_tooltip = &m_application.add_tooltip(std::make_unique<Tooltip>(m_tooltip_text, this, position()));
                // std::cout << m_tooltip << std::endl;
                m_tooltip_counter = -1;
            }
        }
        else if (m_tooltip_counter == 0) {
            // std::cout << "TEST " << this << " " << m_tooltip << std::endl;
            m_application.remove_tooltip(m_tooltip);
            m_tooltip = nullptr;
            m_tooltip_counter = -1;
        }
    }
}

void Widget::do_handle_event(Event& event) {
    Widget::handle_event(event);
    handle_event(event);
}

void Widget::do_update() {
    Widget::update();
    update();
}

void Widget::set_focused() {
    m_application.set_focused_widget(this);
}

bool Widget::is_focused() const {
    return m_application.focused_widget() == this;
}

bool Widget::are_all_parents_enabled() const {
    return is_enabled() && (m_parent ? m_parent->is_enabled() : true);
}

void Widget::handle_event(Event& event) {
    if (event.type() == sf::Event::MouseMoved) {
        sf::Vector2i mouse_pos { event.event().mouseMove.x, event.event().mouseMove.y };
        bool previous_hover = m_hover;
        m_hover = is_mouse_over(mouse_pos);
        if (previous_hover != m_hover) {
            if (m_tooltip)
                m_tooltip_counter = 30;
            else
                m_tooltip_counter = 90;
            event.set_seen();
        }
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
    /*
    sf::RectangleShape outline(size());
    outline.setOutlineThickness(-1);
    outline.setOutlineColor(sf::Color::Red);
    outline.setFillColor(sf::Color::Transparent);
    window.draw(outline);
    */
}

void Widget::do_draw(sf::RenderWindow& window) const {
    sf::View clip_view { sf::FloatRect { {}, size() } };
    auto window_size = window.getSize();
    clip_view.setViewport(sf::FloatRect {
        position().x / window_size.x, position().y / window_size.y,
        size().x / window_size.x, size().y / window_size.y });
    window.setView(clip_view);

    this->draw(window);
    Widget::draw(window);
}

void Widget::do_relayout() {
    if (this->m_visible)
        this->relayout();
    //std::cout << "do_relayout "  << this << ":" << typeid(*this).name() << m_size.x << "," << m_size.y << "@" << m_pos.x << "," << m_pos.y << std::endl;
}

void Widget::set_needs_relayout() {
    m_application.set_needs_relayout();
}

sf::RenderWindow& Widget::window() const {
    return m_application.window();
}

void Widget::dump(unsigned depth) {
    for (int i = 0; i < depth; i++)
        std::cout << "-   ";
    std::cout << typeid(*this).name() << ": pos=" << m_pos.x << "," << m_pos.y << " size=" << m_size.x << "," << m_size.y << std::endl;
}
