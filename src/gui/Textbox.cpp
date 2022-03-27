#include "Textbox.hpp"
#include "../World.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>

Textbox::Textbox(std::string text, sf::IntRect rect)
: m_content(text), m_rect(rect) {}

void Textbox::set_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color)
{
    m_bg_color = bg_color;
    m_fg_color = fg_color;
    m_text_color = text_color;
}

void Textbox::get_events(sf::Event& event)
{
    auto pos = sf::Mouse::getPosition();
    if((m_rect.left >= pos.x && m_rect.top >= pos.y) || (m_rect.left + m_rect.width <= pos.x && m_rect.top + m_rect.height <= pos.y))
    {
        if(event.type == sf::Event::MouseButtonPressed)
        {
            focused = false;
        }
    }
    else if(event.type == sf::Event::MouseButtonPressed)
    {
        focused = true;
    }
    else if(event.type == sf::Event::TextEntered)
    {
        if(event.text.unicode == 8)
        {
            m_content = m_content.substr(0, m_content.size() - 1);
        }
        else if(event.text.unicode <= 128)
        {
            if(m_content.size() < m_limit)
                m_content += static_cast<char>(event.text.unicode);
        }
    }
}

void Textbox::draw(sf::RenderWindow& window)
{
    sf::RectangleShape rect;
    rect.setPosition(sf::Vector2f(m_rect.left, m_rect.top));
    rect.setSize(sf::Vector2f(m_rect.width, m_rect.height));
    rect.setFillColor(m_bg_color);

    if(focused)
    {
        rect.setOutlineColor(m_fg_color);
        rect.setOutlineThickness(3);
    }

    window.draw(rect);

    sf::Text text(m_content, World::font, m_rect.height - 4);
    text.setFillColor(m_text_color);
    text.setPosition(sf::Vector2f(m_rect.left + 2, m_rect.top + 2));

    window.draw(text);
}
