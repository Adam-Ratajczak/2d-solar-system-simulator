#include "Slider.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cmath>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <iostream>
#include "../Vector2.hpp"

Slider::Slider(double min_val, double max_val, double step): 
                m_min_val(min_val), 
                m_max_val(max_val), 
                m_step(step), 
                m_mode(0),
                m_val((min_val + max_val) / 2){

}

Slider::Slider(double min_val, double max_val):
                m_min_val(min_val),
                m_max_val(max_val),
                m_step(1),
                m_mode(1),
                m_val((min_val + max_val) / 2){

}

double Slider::get_value() const{
    return m_val;
}

double& Slider::set_value(const double val){
    if(val < m_min_val)
        m_val = m_min_val;
    else if(val > m_max_val)
        m_val = m_max_val;
    else
        m_val = val;
    return m_val;
}

void Slider::set_display_attributes(sf::Color bg_color, sf::Color fg_color){
    m_bg_color = bg_color;
    m_fg_color = fg_color;
}

void Slider::set_size(unsigned len, unsigned width){
    m_len = len;
    m_width = width;
}

void Slider::set_text_attributes(unsigned text_size, std::string string, TextPos text_pos){
    m_text_size = text_size;
    m_string = string;
    m_text_pos = text_pos;
}

void Slider::set_position(const sf::Vector2f new_pos){
    m_pos = new_pos;
}

Vector2 mouse_pos;

void Slider::draw(sf::RenderWindow& window){
    float posx, posy;
    if(m_mode)
        posx = m_pos.x + std::log10(m_val - m_min_val) / std::log10(m_max_val - m_min_val) * m_width;
    else
        posx = m_pos.x + (m_val - m_min_val) / (m_max_val - m_min_val) * m_len;
    posy = m_pos.y - m_width * 2;

    /*sf::Event event;
    while (window.pollEvent(event)) {
        if(event.type == sf::Event::MouseButtonPressed){
            if(mouse_pos.x >= posx && mouse_pos.x <= posx + m_width && mouse_pos.y >= posy && mouse_pos.y <= posy + m_width * 5){
                mouse_pos = sf::Mouse::getPosition();
                m_dragging = true;
                std::cout << "XD\n";
            }
        }else if(event.type == sf::Event::MouseButtonReleased){
            m_dragging = false;
        }else if(sf::Event::MouseMoved){
            if(m_dragging){
                float delta_pos = sf::Mouse::getPosition().x - mouse_pos.x;
                unsigned count = (m_max_val - m_min_val) / m_step;
                float step = m_len / m_step;
                m_val += step * count;
            }
        }
    }*/

    sf::RectangleShape slider;
    slider.setSize(sf::Vector2f(m_len, m_width));
    slider.setPosition(m_pos);
    slider.setFillColor(m_bg_color);
    window.draw(slider);

    // std::cout << slider.getPosition().x << " " << slider.getPosition().y << "\n";

    sf::RectangleShape bound;
    bound.setSize(sf::Vector2f(m_width, m_width * 3));
    bound.setFillColor(m_bg_color);
    bound.setPosition(m_pos.x, m_pos.y - m_width);
    window.draw(bound);
    bound.setPosition(m_pos.x + m_len - m_width, m_pos.y - m_width);
    window.draw(bound);

    sf::RectangleShape slider_value;
    slider_value.setSize(sf::Vector2f(m_width, m_width * 5));
    slider_value.setFillColor(m_fg_color);

    if(m_mode)
        slider_value.setPosition(m_pos.x + std::log10(m_val - m_min_val) / std::log10(m_max_val - m_min_val) * m_width, m_pos.y - m_width * 2);
    else
        slider_value.setPosition(m_pos.x + (m_val - m_min_val) / (m_max_val - m_min_val) * m_len, m_pos.y - m_width * 2);
    window.draw(slider_value);
    // std::cout << "XD\n";
}

