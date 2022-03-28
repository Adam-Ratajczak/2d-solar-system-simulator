#include "Textbox.hpp"
#include "../World.hpp"
#include "GUI.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>

void Textbox::set_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color)
{
    m_bg_color = bg_color;
    m_fg_color = fg_color;
    m_text_color = text_color;
}

void Textbox::handle_event(Event& event)
{
    auto pos = sf::Mouse::getPosition();
    if(event.type() == sf::Event::TextEntered)
    {
        if(is_focused())
        {
            if(event.event().text.unicode == '\b')
            {
                m_content = m_content.substr(0, m_content.size() - 1);
            }
            else if(event.event().text.unicode < 128 && event.event().text.unicode >= 32)
            {
                if(m_content.size() < m_limit)
                    m_content += static_cast<char>(event.event().text.unicode);
            }
            event.set_handled();
        }
    }
}

void Textbox::draw(sf::RenderWindow& window) const
{
    sf::RectangleShape rect(size());
    rect.setPosition(position());
    rect.setFillColor(m_bg_color);

    if(is_focused())
    {
        rect.setOutlineColor(m_fg_color);
        rect.setOutlineThickness(3);
    }

    window.draw(rect);

    sf::Text text(m_content, GUI::font, size().y - 4);
    text.setFillColor(m_text_color);
    text.setPosition(sf::Vector2f(position().x + 2, position().y + 2));

    window.draw(text);
}
