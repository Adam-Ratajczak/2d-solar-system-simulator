#include "EssaGUI.hpp"
#include "gui/ValueSlider.hpp"
#include <memory>

#include <iostream>

void EssaGUI::m_create_object_gui(GUI::Container& container) {
    auto label = container.add_widget<GUI::Textfield>();
    label->set_content("Create Object");
    label->set_size({ Length::Auto, 30.0_px });
    label->set_alignment(GUI::Align::Center);

    m_radius_control = container.add_widget<GUI::ValueSlider>(0, 500000);
    m_radius_control->set_name("Radius");
    m_radius_control->set_unit("km");
    m_radius_control->on_change = [this](auto) {
        m_forward_simulation_is_valid = false;
    };

    auto mass_container = container.add_widget<GUI::Container>();
    {
        auto& mass_layout = mass_container->set_layout<GUI::HorizontalBoxLayout>();
        mass_layout.set_spacing(10);

        auto mass_textfield = mass_container->add_widget<GUI::Textfield>();
        mass_textfield->set_size({ 150.0_px, Length::Auto });
        mass_textfield->set_content("Mass: ");
        mass_textfield->set_alignment(GUI::Align::CenterLeft);

        m_mass_textbox = mass_container->add_widget<GUI::Textbox>();
        m_mass_textbox->set_limit(6);
        m_mass_textbox->set_content("1.0");
        m_mass_textbox->on_change = [this](auto) {
            m_forward_simulation_is_valid = false;
        };

        auto mass_value_container = mass_container->add_widget<GUI::Container>();
        mass_value_container->set_layout<GUI::HorizontalBoxLayout>();
        {
            auto mass_exponent_textfield = mass_value_container->add_widget<GUI::Textfield>();
            mass_exponent_textfield->set_content(" * 10 ^ ");
            mass_exponent_textfield->set_alignment(GUI::Align::CenterLeft);

            m_mass_exponent_textbox = mass_value_container->add_widget<GUI::Textbox>();
            m_mass_exponent_textbox->set_limit(2);
            m_mass_exponent_textbox->set_content("1");
            m_mass_exponent_textbox->on_change = [this](auto) {
                m_forward_simulation_is_valid = false;
            };

            auto mass_unit_textfield = mass_value_container->add_widget<GUI::Textfield>();
            mass_unit_textfield->set_content(" kg ");
            mass_unit_textfield->set_alignment(GUI::Align::Center);
        }
        mass_layout.set_multipliers({ 5.f / 3, 5.f / 3, 5.f / 9, 5.f / 9, 5.f / 9 });
    }
}

std::shared_ptr<GUI::Container> EssaGUI::m_create_object_from_params_gui(std::shared_ptr<GUI::Container> parent) {
    auto container = std::make_shared<GUI::Container>(*parent);
    container->set_layout<GUI::VerticalBoxLayout>().set_spacing(5);

    m_velocity_control = container->add_widget<GUI::ValueSlider>(0, 500000);
    m_velocity_control->set_name("Velocity");
    m_velocity_control->set_unit("m/s");
    m_velocity_control->on_change = [this](double) {
        m_forward_simulation_is_valid = false;
    };

    m_direction_control = container->add_widget<GUI::ValueSlider>(0, 360, 0.1);
    m_direction_control->set_name("Direction");
    m_direction_control->set_unit("[deg]");
    m_direction_control->slider();
    m_direction_control->on_change = [this](double) {
        m_forward_simulation_is_valid = false;
    };
    return container;
}

std::shared_ptr<GUI::Container> EssaGUI::m_create_object_from_orbit_gui(std::shared_ptr<GUI::Container> parent) {
    auto container = std::make_shared<GUI::Container>(*parent);
    container->set_layout<GUI::VerticalBoxLayout>().set_spacing(5);

    m_apogee_control = container->add_widget<GUI::ValueSlider>(0, 1e10, 100);
    m_apogee_control->set_name("Apogee");
    m_apogee_control->set_unit("km");

    m_perigee_control = container->add_widget<GUI::ValueSlider>(0, 1e10, 100);
    m_perigee_control->set_name("Perigee");
    m_perigee_control->set_unit("km");

    m_orbit_angle_control = container->add_widget<GUI::ValueSlider>(0, 360, 1);
    m_orbit_angle_control->set_name("Angle");
    m_orbit_angle_control->set_unit("[deg]");

    m_orbiit_tilt_control = container->add_widget<GUI::ValueSlider>(0, 360, 1);
    m_orbiit_tilt_control->set_name("Tilt");
    m_orbiit_tilt_control->set_unit("[deg]");

    return container;
}

std::unique_ptr<Object> EssaGUI::m_create_object_from_params() const {
    double mass = std::stod(m_mass_textbox->get_content().toAnsiString()) * std::pow(10, std::stod(m_mass_exponent_textbox->get_content().toAnsiString()));
    double radius = m_radius_control->value() * 1000;

    double theta = m_direction_control->value() / 360 * 2 * M_PI;
    double velocity = m_velocity_control->value();
    if (m_units)
        velocity /= 3.6;
    Vector3 vel(std::cos(theta) * velocity, std::sin(theta) * velocity);

    sf::Color color = m_color_control->value();
    // std::cout << (int)color.r << "," << (int)color.g << "," << (int)color.b << "," << (int)color.a << std::endl;

    std::string name = m_name_textbox->get_content();

    return std::make_unique<Object>(m_world, mass, radius, m_new_object_pos, vel, color, name, 1000);
}
