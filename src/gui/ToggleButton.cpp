#include "ToggleButton.hpp"

// FIXME: SFML doesn't define move constructors anyway, the copy will be made :(
ToggleButton::ToggleButton(Container& c, sf::Image img)
: Button(c, std::move(img)) {}

void ToggleButton::on_click_impl()
{
    Button::on_click_impl();
    set_active(!m_active);
}

sf::Color ToggleButton::color_for_state() const
{
    sf::Color base_color = is_active() ? sf::Color(0, 80, 255) : sf::Color(92, 89, 89);
    if(is_hover())
        base_color += sf::Color { 50, 50, 50 };
    return base_color;
}
