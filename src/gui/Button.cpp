#include "Button.hpp"
#include "../World.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>

Button::Button(Container* c, sf::Vector2f pos, sf::Image img, float scale)
: Widget(c)
{
    constexpr int STATES = 4;

    auto width = img.getSize().x / STATES;
    auto height = img.getSize().y;

    // std::cout << width << ", " << height << " NIGGER\n";

    for(unsigned i = 0; i < STATES; i++)
    {
        sf::Texture texture;
        texture.loadFromImage(img, sf::IntRect(width * i, 0, width, height));
        m_tex.push_back(texture);
    }

    set_position(pos);
    set_size({ width * scale, height * scale });
    m_scale = scale;
}

void Button::handle_event(sf::Event& event)
{
    Widget::handle_event(event);
    if(is_hover())
    {
        if(event.type == sf::Event::MouseButtonReleased)
        {
            m_active = !m_active;
            on_change(m_active);
        }
        // FIXME: VERY HACKY REMOVE THAT ASAP
        World::dragging = false;
    }
}

void Button::draw(sf::RenderWindow& window) const
{
    sf::Sprite sprite;
    sprite.setTexture(m_tex[state_to_texture_index()]);
    sprite.setPosition(position());

    sprite.setScale(sf::Vector2f(m_scale, m_scale));

    window.draw(sprite);
}
