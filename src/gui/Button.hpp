#pragma once

#include "../Constants.hpp"
#include "Widget.hpp"
#include <SFML/Graphics.hpp>

class Button : public Widget
{
    std::vector<sf::Texture> m_tex;
    float m_scale = 1;
    bool m_active = false;

    int state_to_texture_index() const { return (m_active << 1) | is_hover(); }

public:
    Button(sf::Vector2f pos, sf::Image img, float scale);
    Button() = default;

    bool is_active() const { return m_active; }

    virtual void handle_event(sf::Event&) override;
    virtual void draw(sf::RenderWindow& window) const override;
};
