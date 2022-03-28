#pragma once

#include "Button.hpp"
#include "Container.hpp"
#include "SimulationView.hpp"
#include "Slider.hpp"
#include "Textbox.hpp"
#include "Textfield.hpp"
#include "ToggleButton.hpp"
#include "Widget.hpp"

#include <SFML/Graphics.hpp>
#include <random>

class GUI : public Container
{
public:
    explicit GUI(World& world, Root& root);
    static sf::Font font;

    virtual void handle_event(Event&) override;
    virtual void draw(sf::RenderWindow& window) const override;

private:
    virtual void relayout() override;

    std::shared_ptr<ToggleButton> m_create_button;
    std::shared_ptr<Button> m_home_button;
    std::shared_ptr<Button> m_coords_button;
    std::shared_ptr<Button> m_add_object_button;

    std::shared_ptr<Textbox> m_mass_textbox;
    std::shared_ptr<Textbox> m_mass_exponent_textbox;
    std::shared_ptr<Textbox> m_velocity_textbox;
    std::shared_ptr<Textbox> m_radius_textbox;
    std::shared_ptr<Textbox> m_name_textbox;

    std::shared_ptr<Slider> m_direction_slider;
    std::shared_ptr<Textfield> direction_unit_textfield;

    std::shared_ptr<Slider> m_color_r_slider;
    std::shared_ptr<Slider> m_color_g_slider;
    std::shared_ptr<Slider> m_color_b_slider;

    std::shared_ptr<SimulationView> m_simulation_view;

    sf::Vector2f m_new_object_pos;
};
