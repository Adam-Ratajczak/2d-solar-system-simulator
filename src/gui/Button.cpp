#include "Button.hpp"
#include "GUI.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include "../World.hpp"

Button::Button(sf::Vector2f pos, sf::Image img, unsigned states, float scale){
    auto width = img.getSize().x / states;
    auto height = img.getSize().y;

    // std::cout << width << ", " << height << " NIGGER\n";

    for(unsigned i = 0; i < states; i++){
        sf::Texture texture;
        texture.loadFromImage(img, sf::IntRect(width * i, 0, width, height));
        m_tex.push_back(texture);
    }

    m_pos = pos;
    m_size.x = width * scale;
    m_size.y = height * scale;

    m_scale = scale;
}

// template<typename lambda>
void Button::event_listener(sf::Event& event){
    if(event.type == sf::Event::MouseMoved) {
        sf::Vector2i mouse_pos {event.mouseMove.x, event.mouseMove.y};
        m_hover = is_mouse_over(mouse_pos);
    }
    
    if(m_hover) {
        if(event.type == sf::Event::MouseButtonReleased)
            m_active = !m_active;
        // FIXME: VERY HACKY REMOVE THAT ASAP
        World::dragging = false;
    }
}

void Button::draw(sf::RenderWindow &window){
    sf::Sprite sprite;
    sprite.setTexture(m_tex[state_to_texture_index()]);
    sprite.setPosition(m_pos);

    sprite.setScale(sf::Vector2f(m_scale, m_scale));

    window.draw(sprite);
}

bool Button::is_mouse_over(sf::Vector2i mouse_pos) const {
    return sf::Rect<float>(m_pos, m_size).contains(mouse_pos.x, mouse_pos.y);
}
