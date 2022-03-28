#pragma once

#include "../Constants.hpp"
#include "Widget.hpp"
#include <SFML/Graphics.hpp>
#include <functional>

class Button : public Widget
{
    sf::Texture m_texture;
    float m_scale = 1;
    bool m_active = false;

public:
    Button(Container*, sf::Image);

    bool is_active() const { return m_active; }
    void set_active(bool active) { m_active = active; on_change(active); }

    std::function<void(bool)> on_change;

    virtual void handle_event(sf::Event&) override;
    virtual void draw(sf::RenderWindow& window) const override;
};
