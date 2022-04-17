#pragma once

#include "Object.hpp"
#include "SimulationView.hpp"
#include "World.hpp"
#include "gui/Button.hpp"
#include "gui/ColorPicker.hpp"
#include "gui/Container.hpp"
#include "gui/SettingsMenu.hpp"
#include "gui/Slider.hpp"
#include "gui/TextButton.hpp"
#include "gui/StateTextButton.hpp"
#include "gui/Textbox.hpp"
#include "gui/Textfield.hpp"
#include "gui/ValueSlider.hpp"
#include "gui/Widget.hpp"

#include <SFML/Graphics.hpp>
#include <memory>
#include <random>
#include <vector>

sf::Image load_image(std::string path);

class EssaGUI : public GUI::Container {
public:
    explicit EssaGUI(GUI::Application& application, World& world);

    virtual void handle_event(GUI::Event&) override;
    virtual void draw(sf::RenderWindow& window) const override;

    SimulationView& simulation_view() const { return *m_simulation_view; }

private:
    World& m_world;
    virtual void relayout() override;
    virtual void update() override;

    std::shared_ptr<GUI::ImageButton> m_toggle_orbit_direction_button;
    std::shared_ptr<GUI::ImageButton> m_toggle_unit_button;
    std::shared_ptr<GUI::ImageButton> m_toggle_camera_mode_at_focus;

    std::shared_ptr<GUI::Button> m_home_button;
    std::shared_ptr<GUI::Button> m_coords_button;
    std::shared_ptr<GUI::Button> m_add_object_button;
    std::shared_ptr<GUI::Button> m_canvas_mode_button;

    std::shared_ptr<GUI::ValueSlider> m_radius_control;

    std::shared_ptr<GUI::ValueSlider> m_velocity_control;
    std::shared_ptr<GUI::ValueSlider> m_direction_xz_control;
    std::shared_ptr<GUI::ValueSlider> m_direction_yz_control;
    std::shared_ptr<GUI::ValueSlider> m_y_position_control;

    std::shared_ptr<GUI::ValueSlider> m_orbit_angle_control;
    std::shared_ptr<GUI::ValueSlider> m_orbit_tilt_control;
    std::shared_ptr<GUI::ValueSlider> m_apogee_control;
    std::shared_ptr<GUI::ValueSlider> m_perigee_control;

    std::shared_ptr<GUI::Textbox> m_mass_textbox;
    std::shared_ptr<GUI::Textbox> m_mass_exponent_textbox;
    std::shared_ptr<GUI::Textbox> m_name_textbox;

    std::shared_ptr<GUI::ColorPicker> m_color_control;

    std::shared_ptr<Container> m_create_object_from_params_container;
    std::shared_ptr<Container> m_create_object_from_orbit_container;
    std::shared_ptr<Container> m_focused_object_info;

    std::shared_ptr<Container> m_submit_container;

    std::shared_ptr<SimulationView> m_simulation_view;
    
    Object* m_focused = nullptr;
    bool m_automatic_orbit_calculation = false;

    std::shared_ptr<GUI::ImageButton> m_create_toggle_unit_button();
    std::shared_ptr<Container> m_create_object_from_params_gui(std::shared_ptr<Container> parent);
    std::shared_ptr<Container> m_create_object_from_orbit_gui(std::shared_ptr<Container> parent);
    std::shared_ptr<Container> m_create_focused_object_info_gui();
    void m_update_focused_object_info_gui(Object* focused);
    void m_switch_info(bool state);

    std::vector<std::shared_ptr<GUI::Textfield>> m_info_content;

    std::unique_ptr<Object> m_create_object_from_params() const;
    std::unique_ptr<Object> m_create_object_from_orbit() const;
    void m_create_simulation_settings_gui(Container& container);
    void m_create_object_gui(Container& container);
    void m_create_cavas_mode_gui(Container& parent);

    World m_forward_simulated_world;
    bool m_forward_simulation_is_valid = true;
    std::unique_ptr<Object> m_new_object;
    Vector3 m_new_object_pos;
    int m_saved_speed = 0;
    void m_recalculate_forward_simulation();
};
