#include "GUI.hpp"

void GUI::m_create_object_from_params_gui(std::shared_ptr<Container> container) {
    m_radius_control = container->add_widget<ValueSlider>(0, 500000);
    m_radius_control->set_name("Radius");
    m_radius_control->set_unit("km");

    m_velocity_control = container->add_widget<ValueSlider>(0, 500000);
    m_velocity_control->set_name("Velocity");
    m_velocity_control->set_unit("m/s");

    m_direction_control = container->add_widget<ValueSlider>(0, 360, 0.1);
    m_direction_control->set_name("Direction");
    m_direction_control->set_unit("[deg]");
    m_direction_control->slider().set_wraparound(true);
}

Object GUI::m_create_object_from_params() const {
    double mass = std::stod(m_mass_textbox->get_content().toAnsiString()) * std::pow(10, std::stod(m_mass_exponent_textbox->get_content().toAnsiString()));
    double radius = m_radius_control->value() * 1000;

    double theta = m_direction_control->value() / 360 * 2 * M_PI;
    double velocity = m_velocity_control->value();
    Vector2 vel(std::cos(theta) * velocity, std::sin(theta) * velocity);

    sf::Color color(
        m_color_r_slider->get_value(),
        m_color_g_slider->get_value(),
        m_color_b_slider->get_value());

    std::string name = m_name_textbox->get_content();

    return Object(m_world, mass, radius, m_new_object_pos, vel, color, name, 1000);
}
