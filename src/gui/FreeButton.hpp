#pragma once

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include "../Constants.hpp"

class FreeButton{
    sf::Vector2f m_pos, m_size;
    std::vector<sf::Texture> m_tex;
    unsigned m_mode = 0;
    float m_scale;
public:
    FreeButton(sf::Vector2f pos, sf::Image img, unsigned states, float scale);
    FreeButton() = default;

// template<typename lambda>
    void event_listener(sf::Event& event, EventListening mode);

    void draw(sf::RenderWindow& window);
};
