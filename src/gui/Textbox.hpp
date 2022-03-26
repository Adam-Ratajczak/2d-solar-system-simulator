#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <string>
class Textbox{
    std::string m_content;
    sf::Color m_bg_color, m_fg_color, m_text_color;
    sf::IntRect m_rect;
    bool focused = false;
    sf::Clock timer;
    unsigned m_limit = 16;

public:
    Textbox(std::string text, sf::IntRect rect);

    void set_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color);
    void get_events(sf::Event& event);
    void draw(sf::RenderWindow& window);
    void set_limit(unsigned limit){m_limit = limit;}

    std::string get_content()const{return m_content;}
};
