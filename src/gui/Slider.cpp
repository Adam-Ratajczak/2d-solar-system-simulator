#include "Slider.hpp"
#include "../Vector2.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cmath>
#include <iostream>

Slider::Slider(Container* c, double min_val, double max_val, double step)
: Widget(c), m_min_val(min_val), m_max_val(max_val), m_step(step), m_val((min_val + max_val) / 2)
{
}

double Slider::get_value() const
{
    switch(m_mode)
    {
        case Mode::Linear: return m_val;
        case Mode::Exponential: return std::pow(m_exponent, m_val);
    }
    return 0;
}

void Slider::set_value(double val)
{
    if(val < m_min_val)
        m_val = m_min_val;
    else if(val > m_max_val)
        m_val = m_max_val;
    else
        m_val = val;
}

void Slider::set_display_attributes(sf::Color bg_color, sf::Color fg_color)
{
    m_bg_color = bg_color;
    m_fg_color = fg_color;
}

void Slider::set_text_attributes(unsigned text_size, std::string string, TextPos text_pos)
{
    m_text_size = text_size;
    m_string = string;
    m_text_pos = text_pos;
}

void Slider::handle_event(sf::Event& event)
{
    if(event.type == sf::Event::MouseButtonPressed)
    {
        if(is_mouse_over({event.mouseButton.x, event.mouseButton.y}))
            m_dragging = true;
    }
    else if(event.type == sf::Event::MouseButtonReleased)
    {
        m_dragging = false;
    }
    else if(sf::Event::MouseMoved)
    {
        if(m_dragging)
        {
            auto mouse_pos_relative_to_slider = sf::Vector2f({static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y)}) - position();
            m_val = (mouse_pos_relative_to_slider.x / size().x) * (m_max_val - m_min_val) + m_min_val;
            m_val = std::min(std::max(m_min_val, m_val), m_max_val);

            // round to step
            m_val /= m_step;
            m_val = std::round(m_val);
            m_val *= m_step;

            std::cout << get_value() << std::endl;
        }
    }
}

float Slider::calculate_knob_size() const
{
    return std::max(4.0, size().x / (m_max_val - m_min_val) * m_step);
}

void Slider::draw(sf::RenderWindow& window) const
{
    sf::RectangleShape slider({size().x, 5.f});
    slider.setPosition(position().x, position().y + size().y / 2 - 2.5f);
    slider.setFillColor(m_bg_color);
    window.draw(slider);

    // std::cout << slider.getPosition().x << " " << slider.getPosition().y << "\n";

    sf::RectangleShape bound;
    bound.setSize(sf::Vector2f(2, 10));
    bound.setFillColor(m_bg_color);
    bound.setPosition(position().x, position().y + size().y / 2 - 5);
    window.draw(bound);
    bound.setPosition(position().x + size().x - 2, position().y + size().y / 2 - 5);
    window.draw(bound);

    sf::RectangleShape slider_value;
    auto knob_size_x = calculate_knob_size();
    slider_value.setSize(sf::Vector2f(knob_size_x, 20.f));
    slider_value.setFillColor(m_fg_color);

    slider_value.setPosition(position().x + (m_val - m_min_val) / (m_max_val - m_min_val) * size().x - knob_size_x / 2, position().y + size().y / 2 - 10.f);
    window.draw(slider_value);
    // std::cout << "XD\n"
}
