#include "Slider.hpp"
#include "../Vector2.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cmath>
#include <iostream>

Slider::Slider(GUI& gui, double min_val, double max_val, double step)
: Widget(gui), m_min_val(min_val), m_max_val(max_val), m_step(step), m_mode(0), m_val((min_val + max_val) / 2)
{
}

double Slider::get_value() const
{
    return m_val;
}

double& Slider::set_value(const double val)
{
    if(val < m_min_val)
        m_val = m_min_val;
    else if(val > m_max_val)
        m_val = m_max_val;
    else
        m_val = val;
    return m_val;
}

void Slider::set_display_attributes(sf::Color bg_color, sf::Color fg_color)
{
    m_bg_color = bg_color;
    m_fg_color = fg_color;
}

void Slider::set_size(unsigned len, unsigned width)
{
    m_len = len;
    m_width = width;
}

void Slider::set_text_attributes(unsigned text_size, std::string string, TextPos text_pos)
{
    m_text_size = text_size;
    m_string = string;
    m_text_pos = text_pos;
}

Vector2 mouse_pos;

void Slider::handle_event(sf::Event& event)
{
    // TODO: Make it working.
    float posx, posy;
    if(m_mode)
        posx = position().x + std::log10(m_val - m_min_val) / std::log10(m_max_val - m_min_val) * m_width;
    else
        posx = position().x + (m_val - m_min_val) / (m_max_val - m_min_val) * m_len;
    posy = position().y - m_width * 2;

    if(event.type == sf::Event::MouseButtonPressed)
    {
        if(mouse_pos.x >= posx && mouse_pos.x <= posx + m_width && mouse_pos.y >= posy && mouse_pos.y <= posy + m_width * 5)
        {
            mouse_pos = sf::Mouse::getPosition();
            m_dragging = true;
            // std::cout << "XD\n";
        }
    }
    else if(event.type == sf::Event::MouseButtonReleased)
    {
        m_dragging = false;
    }
    else if(sf::Event::MouseMoved)
    {
        if(m_dragging)
        {
            float delta_pos = sf::Mouse::getPosition().x - mouse_pos.x;
            unsigned count = (m_max_val - m_min_val) / m_step;
            float step = m_len / m_step;
            m_val += step * count;
        }
    }
}

void Slider::draw(sf::RenderWindow& window) const
{
    sf::RectangleShape slider;
    slider.setSize(sf::Vector2f(m_len, m_width));
    slider.setPosition(position());
    slider.setFillColor(m_bg_color);
    window.draw(slider);

    // std::cout << slider.getPosition().x << " " << slider.getPosition().y << "\n";

    sf::RectangleShape bound;
    bound.setSize(sf::Vector2f(m_width, m_width * 3));
    bound.setFillColor(m_bg_color);
    bound.setPosition(position().x, position().y - m_width);
    window.draw(bound);
    bound.setPosition(position().x + m_len - m_width, position().y - m_width);
    window.draw(bound);

    sf::RectangleShape slider_value;
    slider_value.setSize(sf::Vector2f(m_width, m_width * 5));
    slider_value.setFillColor(m_fg_color);

    if(m_mode)
        slider_value.setPosition(position().x + std::log10(m_val - m_min_val) / std::log10(m_max_val - m_min_val) * m_width, position().y - m_width * 2);
    else
        slider_value.setPosition(position().x + (m_val - m_min_val) / (m_max_val - m_min_val) * m_len, position().y - m_width * 2);
    window.draw(slider_value);
    // std::cout << "XD\n";
}
