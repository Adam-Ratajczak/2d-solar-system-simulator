#pragma once

#include "Widget.hpp"

#include <SFML/Graphics.hpp>
#include <string>

class Textbox : public Widget
{
    std::string m_content;
    sf::Color m_bg_color, m_fg_color, m_text_color;

    // TODO: Move this to Widget.
    bool focused = false;

    unsigned m_limit = 16;

public:
    Textbox(Container* c)
    : Widget(c) {}

    void set_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color);
    virtual void handle_event(sf::Event& event) override;
    virtual void draw(sf::RenderWindow& window) const override;
    void set_limit(unsigned limit) { m_limit = limit; }

    std::string get_content() const { return m_content; }
};
