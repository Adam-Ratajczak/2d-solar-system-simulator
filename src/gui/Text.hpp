#pragma once
#include "Widget.hpp"
#include <SFML/Graphics.hpp>

class Text : public Widget{
public:
    enum class Align{
        LEFT,
        CENTER,
        RIGHT
    };
private:
    std::string m_content;
    sf::Color m_bg_color, m_fg_color, m_text_color;
    unsigned m_font_size = 20;
    Align m_alignment = Align::LEFT;
public:
    Text(Container* c, sf::IntRect rect);

    void set_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color);
    virtual void handle_event(sf::Event& event) override;
    virtual void draw(sf::RenderWindow& window) const override;

    std::string get_content() const { return m_content; }
    void set_content(std::string content) { m_content = content; }

    unsigned get_font_size() const { return m_font_size; }
    void set_font_size(unsigned font_size) { m_font_size = font_size; }

    Align get_alignment() const { return m_alignment; };
    void set_alignment(Align alignment){ m_alignment = alignment; };
};
