#include "FreeButton.hpp"
#include "GUI.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include "../World.hpp"

FreeButton::FreeButton(sf::Vector2f pos, sf::Image img, unsigned states, float scale){
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
void FreeButton::event_listener(sf::Event& event, EventListening mode){
    auto mouse_pos = sf::Mouse::getPosition();

    if(sf::Rect<float>(m_pos, m_size).contains(mouse_pos.x, mouse_pos.y)){
        if(m_mode == 0)
            m_mode = 1;
        else if(m_mode == 2)
            m_mode = 3;
        switch (mode) {
        case EventListening::ONCLICK:
            if(event.type == sf::Event::MouseButtonPressed){
                // expression();
                if(!drag){
                    if(m_mode == 1)
                        m_mode = 2;
                    else if(m_mode == 3)
                        m_mode = 0;
                    drag = 1;
                }
            }
            break;
        case EventListening::HOVER:
                // expression();
            break;
        case EventListening::ONRELASE:
            if(event.type == sf::Event::MouseButtonReleased){
                // expression();
                drag = 0;
            }
          break;
        }
        World::dragging = 0;
    }else{
        if(m_mode % 2 != 0)
            m_mode--;
    }
}

void FreeButton::draw(sf::RenderWindow &window){
    sf::Sprite sprite;
    sprite.setTexture(m_tex[m_mode]);
    sprite.setPosition(m_pos);

    sprite.setScale(sf::Vector2f(m_scale, m_scale));

    window.draw(sprite);
}
