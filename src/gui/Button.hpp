#pragma once

#include "Widget.hpp"
#include <SFML/Graphics.hpp>
#include <functional>

class Button : public Widget
{
    sf::Texture m_texture;
    bool m_active = false;

public:
    Button(Container&, sf::Image);

    std::function<void()> on_click;

    virtual void handle_event(Event&) override;
    virtual void draw(sf::RenderWindow& window) const override;

protected:
    virtual void on_click_impl() { if(on_click) on_click(); }

private:
    virtual sf::Color color_for_state() const;
};
