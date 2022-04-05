#pragma once

#include "../Object.hpp"
#include "../World.hpp"
#include "Button.hpp"
#include "ColorPicker.hpp"
#include "Container.hpp"
#include "SettingsMenu.hpp"
#include "SimulationView.hpp"
#include "Slider.hpp"
#include "Textbox.hpp"
#include "Textfield.hpp"
#include "ToggleButton.hpp"
#include "ValueSlider.hpp"
#include "Widget.hpp"

#include <SFML/Graphics.hpp>
#include <memory>
#include <random>

class GUI : public Container {
public:
    explicit GUI(Application& application, World& world);
    static sf::Font font;

    virtual void handle_event(Event&) override;
    virtual void draw(sf::RenderWindow& window) const override;

    SimulationView& simulation_view() const { return *m_simulation_view; }

private:
    World& m_world;
    virtual void relayout() override;
    virtual void update() override;

    std::shared_ptr<ToggleButton> m_creative_mode_button;
    std::shared_ptr<ToggleButton> m_toggle_orbit_direction_button;
    std::shared_ptr<ToggleButton> m_toggle_unit_button;

    std::shared_ptr<Button> m_home_button;
    std::shared_ptr<Button> m_coords_button;
    std::shared_ptr<Button> m_add_object_button;

    std::shared_ptr<ValueSlider> m_radius_control;
    std::shared_ptr<ValueSlider> m_velocity_control;
    std::shared_ptr<ValueSlider> m_direction_control;

    // FIXME: Port this all to ValueSlider
    std::shared_ptr<Textbox> m_mass_textbox;
    std::shared_ptr<Textbox> m_mass_exponent_textbox;
    std::shared_ptr<Textbox> m_name_textbox;
    std::shared_ptr<Textbox> m_semi_major_axis_textbox;
    std::shared_ptr<Textbox> m_semi_minor_axis_textbox;

    std::shared_ptr<ColorPicker> m_color_control;

    std::shared_ptr<Container> semi_major_axis_container;
    std::shared_ptr<Container> semi_minor_axis_container;

    std::shared_ptr<Container> m_create_object_from_params_container;
    std::shared_ptr<Container> m_create_object_from_orbit_container;

    std::shared_ptr<Container> m_submit_container;

    std::shared_ptr<SimulationView> m_simulation_view;
    Object* m_focused = nullptr;
    bool m_automatic_orbit_calculation = false, m_units = false;

    std::shared_ptr<Container> m_create_object_from_params_gui(std::shared_ptr<Container> parent);
    std::shared_ptr<Container> m_create_object_from_orbit_gui(std::shared_ptr<Container> parent);
    void m_create_object_gui(Container& container);
    std::unique_ptr<Object> m_create_object_from_params() const;

    World m_forward_simulated_world;
    bool m_forward_simulation_is_valid = true;
    std::unique_ptr<Object> m_new_object;
    Vector3 m_new_object_pos;
    int m_saved_speed = 0;
    void recalculate_forward_simulation();
};
