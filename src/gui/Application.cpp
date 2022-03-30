#include "Application.hpp"

#include "GUI.hpp"

#include <iostream>

void Application::set_focused_widget(Widget* w) {
    m_focused_widget = w;
}

void Application::draw(sf::RenderWindow& window) const {
    window.setView(sf::View{sf::FloatRect(0, 0, window.getSize().x, window.getSize().y)});
    for (auto& tooltip : m_tooltips) {
        sf::Text text(tooltip->text, GUI::font, 15);
        text.setFillColor(sf::Color::Black);
        text.setPosition(tooltip->position);

        auto bounds = text.getGlobalBounds();

        sf::RectangleShape bg { { bounds.width + 10, bounds.height + 10 } };
        auto x_pos = std::min(window.getSize().x - bg.getSize().x, bounds.left - 5);
        bg.setPosition(x_pos, bounds.top - 5);
        text.setPosition(x_pos + 5, text.getPosition().y);
        window.draw(bg);

        window.draw(text);
    }
}

void Application::remove_tooltip(Tooltip* t) {
    std::erase_if(m_tooltips, [&](auto& other_t) { return other_t.get() == t; });
}
