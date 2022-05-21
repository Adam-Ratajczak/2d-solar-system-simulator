#pragma once

#include "../SimulationView.hpp"
#include "../World.hpp"

#include <EssaGUI/gui/ColorPicker.hpp>
#include <EssaGUI/gui/Container.hpp>
#include <EssaGUI/gui/ImageButton.hpp>
#include <EssaGUI/gui/ValueSlider.hpp>
#include <memory>

class EssaCreateObject : public GUI::Container {
public:
    explicit EssaCreateObject(GUI::Container& container, SimulationView& simulation_view);

    void set_new_object(std::unique_ptr<Object> new_obj) { m_new_object = std::move(new_obj); }
    void recalculate_forward_simulation();
    void forward_simulation_state(bool state) { m_forward_simulation_is_valid = state; }

    Object* new_object() { return m_new_object.get(); }
    Object* forward_simulated_new_object() { return m_forward_simulated_new_object; }
    World& forward_simulated_world() { return m_forward_simulated_world; }

    bool is_forward_simulation_valid() const { return m_forward_simulation_is_valid; }
    bool new_object_exist() const { return m_new_object != nullptr; }

private:
    GUI::Button* m_coords_button = nullptr;
    GUI::Button* m_add_object_button = nullptr;

    GUI::ValueSlider* m_forward_simulation_ticks_control = nullptr;
    GUI::ValueSlider* m_radius_control = nullptr;

    GUI::ValueSlider* m_velocity_control = nullptr;
    GUI::ValueSlider* m_direction_xz_control = nullptr;
    GUI::ValueSlider* m_direction_yz_control = nullptr;
    GUI::ValueSlider* m_y_position_control = nullptr;

    GUI::ValueSlider* m_orbit_angle_control = nullptr;
    GUI::ValueSlider* m_orbit_tilt_control = nullptr;
    GUI::ValueSlider* m_apogee_control = nullptr;
    GUI::ValueSlider* m_perigee_control = nullptr;

    GUI::Textbox* m_mass_textbox = nullptr;
    GUI::Textbox* m_mass_exponent_textbox = nullptr;
    GUI::Textbox* m_name_textbox = nullptr;

    GUI::ColorPicker* m_color_control = nullptr;

    std::shared_ptr<Container> m_create_object_from_params_container = nullptr;
    std::shared_ptr<Container> m_create_object_from_orbit_container = nullptr;
    std::shared_ptr<Container> m_submit_create_container = nullptr;
    std::shared_ptr<Container> m_submit_modify_container = nullptr;

    std::shared_ptr<GUI::ImageButton> m_toggle_unit_button = nullptr;
    GUI::ImageButton* m_toggle_orbit_direction_button = nullptr;
    GUI::ImageButton* m_require_orbit_point_button = nullptr;

    World m_forward_simulated_world;
    Vector3 m_new_object_pos;

    std::unique_ptr<Object> m_new_object;

    int m_saved_speed = 0;
    bool m_forward_simulation_is_valid = true;
    bool m_automatic_orbit_calculation = false, m_prev_unit_state = false;

    Object* m_focused = nullptr;
    Object* m_forward_simulated_new_object = nullptr;
    Object* m_to_modify = nullptr;

    SimulationView& m_simulation_view;

    std::shared_ptr<GUI::ImageButton> m_create_toggle_unit_button(Container& parent);
    std::shared_ptr<Container> m_create_object_from_params_gui(Container& parent);
    std::shared_ptr<Container> m_create_object_from_orbit_gui(Container& parent);
    std::shared_ptr<Container> m_create_submit_container(Container& parent);

    void m_create_name_and_color_container();

    std::unique_ptr<Object> m_create_object_from_params() const;
    std::unique_ptr<Object> m_create_object_from_orbit() const;
    void m_create_object_gui(Container& container);
};