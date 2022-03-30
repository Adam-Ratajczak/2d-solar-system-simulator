#pragma once

#include "Button.hpp"
#include "Container.hpp"
#include "SimulationView.hpp"
#include "Slider.hpp"
#include "Textbox.hpp"
#include "Textfield.hpp"
#include "ToggleButton.hpp"
#include "ValueSlider.hpp"
#include "Widget.hpp"
#include "../Object.hpp"

#include <SFML/Graphics.hpp>
#include <memory>
#include <random>

class GUI : public Container {
public:
    explicit GUI(World& world, Application& application);
    static sf::Font font;

    virtual void handle_event(Event&) override;
    virtual void draw(sf::RenderWindow& window) const override;

private:
    World& m_world;
    virtual void relayout() override;

    std::shared_ptr<ToggleButton> m_create_button;
    std::shared_ptr<ToggleButton> m_creative_mode_button;
    std::shared_ptr<Button> m_home_button;
    std::shared_ptr<Button> m_coords_button;
    std::shared_ptr<Button> m_add_object_button;

    // FIXME: Port this all to ValueSlider
    std::shared_ptr<Textbox> m_mass_textbox;
    std::shared_ptr<Textbox> m_mass_exponent_textbox;
    std::shared_ptr<ValueSlider> m_radius_control;
    std::shared_ptr<ValueSlider> m_velocity_control;
    std::shared_ptr<ValueSlider> m_direction_control;
    std::shared_ptr<Textbox> m_name_textbox;

    std::shared_ptr<Slider> m_color_r_slider;
    std::shared_ptr<Slider> m_color_g_slider;
    std::shared_ptr<Slider> m_color_b_slider;

    std::shared_ptr<SimulationView> m_simulation_view;
    mutable std::unique_ptr<Object> m_last_object;

    sf::Vector2f m_new_object_pos;

    void m_create_object_from_params_gui(std::shared_ptr<Container> container);
    std::unique_ptr<Object> m_create_object_from_params() const;
};
