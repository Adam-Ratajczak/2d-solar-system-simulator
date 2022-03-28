#pragma once

#include "Container.hpp"

class Root
{
public:
    explicit Root(sf::RenderWindow& wnd)
    : m_window(wnd) {}

    sf::RenderWindow& window() const { return m_window; }
    Widget* focused_widget() const { return m_focused_widget; }
    void set_focused_widget(Widget* w);

private:
    sf::RenderWindow& m_window;    
    Widget* m_focused_widget {};
};
