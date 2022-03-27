#pragma once

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include "../Constants.hpp"

class Button{
    sf::Vector2f m_pos, m_size;
    std::vector<sf::Texture> m_tex;
    float m_scale;
    bool drag = false;

    bool m_hover = false;
    bool m_active = false;

    int state_to_texture_index() const { return (m_active << 1) | m_hover; }
    bool is_mouse_over(sf::Vector2i) const;

public:
    Button(sf::Vector2f pos, sf::Image img, unsigned states, float scale);
    Button() = default;

    bool is_hover() const { return m_hover; }
    bool is_active() const { return m_active; }

// template<typename lambda>
    void event_listener(sf::Event& event);

    void draw(sf::RenderWindow& window);
};
