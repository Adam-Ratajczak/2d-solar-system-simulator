#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <string>

class Slider{
    double m_val;
    const double m_min_val, m_max_val;
    double m_step;
    bool m_dragging = false;

    const bool m_mode; // 0 for linear, 1 for logarithmic

    sf::Color m_bg_color, m_fg_color, m_text_color;
    unsigned m_len, m_width, m_text_size;
    std::string m_string;
    sf::Vector2f m_pos;

public:
    enum class TextPos{
        TOP, RIGHT, BOTTOM, LEFT
    };

    Slider(double min_val, double max_val, double step);
    Slider(double min_val, double max_val);
    double get_value() const;
    double& set_value(const double val);

    void get_events(sf::Event& event);
    void draw(sf::RenderWindow& window);

    void set_display_attributes(sf::Color bg_color, sf::Color fg_color);
    void set_size(unsigned len, unsigned width);
    void set_text_attributes(unsigned text_size, std::string string, TextPos text_pos = TextPos::RIGHT);
    void set_position(const sf::Vector2f new_pos);

private:
    TextPos m_text_pos;
};
