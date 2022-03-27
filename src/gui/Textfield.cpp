#include "Textfield.hpp"
#include "../World.hpp"
#include "GUI.hpp"
#include <SFML/System/Vector2.hpp>

Textfield::Textfield(Container* c, sf::IntRect rect)
: Widget(c)
{
    set_position({static_cast<float>(rect.left), static_cast<float>(rect.top)});
    set_size({static_cast<float>(rect.width), static_cast<float>(rect.height)});
}

void Textfield::set_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color){
    m_bg_color = bg_color;
    m_fg_color = fg_color;
    m_text_color = text_color;
}

void Textfield::handle_event(sf::Event &event){


}

void Textfield::draw(sf::RenderWindow& window) const{
    sf::RectangleShape rect(size());
    rect.setPosition(position());
    rect.setFillColor(m_bg_color);
    rect.setOutlineColor(m_fg_color);
    rect.setOutlineThickness(3);

    window.draw(rect);

    sf::Text text(m_content, GUI::font, size().y - 4);
    text.setFillColor(m_text_color);
    auto bouds = text.getLocalBounds();

    switch (m_alignment) {
        case Align::LEFT:
            text.setPosition(sf::Vector2f(position().x + 2, position().y + 2));
        break;
        case Align::CENTER:
            text.setPosition(sf::Vector2f(position().x + size().x / 2 - bouds.width / 2, position().y + 2));
        break;
        case Align::RIGHT:
            text.setPosition(sf::Vector2f(position().x + size().x - bouds.width - 2, position().y + 2));
        break;
    }

    window.draw(text);
}
