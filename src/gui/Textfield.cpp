#include "Textfield.hpp"
#include "../World.hpp"
#include "GUI.hpp"
#include <SFML/System/Vector2.hpp>

void Textfield::set_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color){
    m_bg_color = bg_color;
    m_fg_color = fg_color;
    m_text_color = text_color;
}

void Textfield::draw(sf::RenderWindow& window) const{
    sf::RectangleShape rect(size());
    rect.setFillColor(m_bg_color);
    /*rect.setOutlineColor(m_fg_color);
    rect.setOutlineThickness(3);*/

    window.draw(rect);

    sf::Text text(m_content, GUI::font, 15);
    text.setFillColor(m_text_color);
    auto bounds = text.getLocalBounds();

    // The -5 is needed because SFML doesn't take descenders into account
    auto y_center_factor = size().y / 2 - bounds.height / 2 - 5;

    switch (m_alignment) {
        case Align::CenterLeft:
            text.setPosition(sf::Vector2f(2, 2 + y_center_factor ));
        break;
        case Align::Center:
            text.setPosition(sf::Vector2f(size().x / 2 - bounds.width / 2, y_center_factor));
        break;
        case Align::CenterRight:
            text.setPosition(sf::Vector2f(size().x - bounds.width - 2, y_center_factor));
        break;
    }

    window.draw(text);
}
