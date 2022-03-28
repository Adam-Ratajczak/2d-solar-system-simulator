#include "Button.hpp"
#include "../World.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>

Button::Button(Container* c, sf::Image img)
: Widget(c)
{
    constexpr int STATES = 4;

    auto width = img.getSize().x / STATES;
    auto height = img.getSize().y;
    m_texture.loadFromImage(img);
}

void Button::handle_event(sf::Event& event)
{
    Widget::handle_event(event);
    if(is_hover())
    {
        if(event.type == sf::Event::MouseButtonReleased)
        {
            m_active = !m_active;
            on_click_impl();
        }
        // FIXME: VERY HACKY REMOVE THAT ASAP
        World::dragging = false;
    }
}

void Button::draw(sf::RenderWindow& window) const
{
    sf::CircleShape cs_bg(0.5);
    cs_bg.setScale(size());
    cs_bg.setPosition(position());
    cs_bg.setFillColor(color_for_state());
    window.draw(cs_bg);

    sf::Sprite sprite;
    sprite.setTexture(m_texture);
    sprite.setPosition(position());

    auto tex_size = sprite.getTexture()->getSize();
    sprite.setScale(size().x / tex_size.x, size().y / tex_size.y);

    window.draw(sprite);
}

sf::Color Button::color_for_state() const
{
    sf::Color base_color = sf::Color(92, 89, 89);
    if(is_hover())
        base_color += sf::Color{50, 50, 50};
    return base_color;
}
