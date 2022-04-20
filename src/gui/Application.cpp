#include "Application.hpp"

#include <cassert>
#include <iostream>

namespace GUI {

Application* s_the {};

Application& Application::the() {
    assert(s_the);
    return *s_the;
}

Application::Application(sf::RenderWindow& wnd)
    : m_window(wnd) {
    assert(!s_the);
    s_the = this;
    font.loadFromFile("../assets/fonts/Xolonium-pn4D.ttf");
    bold_font.loadFromFile("../assets/fonts/XoloniumBold-xKZO.ttf");
    fixed_width_font.loadFromFile("../assets/fonts/SourceCodePro-Regular.otf");
}

void Application::set_focused_widget(Widget* w) {
    m_focused_widget = w;
}

void Application::draw() {
    if (m_needs_relayout) {
        m_main_widget->do_relayout();
        // m_main_widget->dump(0);
        m_needs_relayout = false;
    }

    m_main_widget->do_draw(m_window);

    m_window.setView(sf::View { sf::FloatRect(0, 0, m_window.getSize().x, m_window.getSize().y) });
    for (auto& tooltip : m_tooltips) {
        sf::Text text(tooltip->text, font, 15);
        text.setFillColor(sf::Color::Black);
        text.setPosition(tooltip->position);

        auto bounds = text.getGlobalBounds();

        sf::RectangleShape bg { { bounds.width + 10, bounds.height + 10 } };
        auto x_pos = std::min(m_window.getSize().x - bg.getSize().x, bounds.left - 5);
        bg.setPosition(x_pos, bounds.top - 5);
        text.setPosition(x_pos + 5, text.getPosition().y);
        m_window.draw(bg);

        m_window.draw(text);
    }
}

void Application::remove_tooltip(Tooltip* t) {
    std::erase_if(m_tooltips, [&](auto& other_t) { return other_t.get() == t; });
}

void Application::handle_events() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        Event gui_event(event);
        m_main_widget->do_handle_event(gui_event);
    }
}

}
