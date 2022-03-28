#pragma once

#include "../World.hpp"
#include "Button.hpp"
#include "Container.hpp"
#include "Slider.hpp"
#include "Textbox.hpp"
#include "Textfield.hpp"
#include "ToggleButton.hpp"
#include "Widget.hpp"

#include <SFML/Graphics.hpp>
#include <random>

class GUI : public Container
{
    std::shared_ptr<ToggleButton> m_create_button;
    std::shared_ptr<Button> m_home_button;
    std::shared_ptr<Button> m_coords_button;

    std::shared_ptr<Textbox> m_mass_textbox;
    std::shared_ptr<Textbox> m_mass_exponent_textbox;
    std::shared_ptr<Textbox> m_velocity_textbox;
    std::shared_ptr<Textbox> m_radius_textbox;

    std::shared_ptr<Slider> m_direction_slider;
    std::shared_ptr<Textfield> direction_unit_textfield;

    std::shared_ptr<Slider> m_color_r_slider;
    std::shared_ptr<Slider> m_color_g_slider;
    std::shared_ptr<Slider> m_color_b_slider;

    sf::RenderWindow& m_window;
    World& m_world;

    Vector2 m_coords;
    double m_mass;

public:
    GUI(World& world);
    static sf::Font font;

private:
    virtual void relayout() override;
};
