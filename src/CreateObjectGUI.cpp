#include "EssaGUI.hpp"
#include "Object.hpp"
#include "gui/Units.hpp"
#include "gui/ValueSlider.hpp"
#include <cmath>
#include <memory>

#include <iostream>

std::shared_ptr<GUI::ImageButton> EssaGUI::m_create_toggle_unit_button(){
    auto button = m_submit_container->add_widget<GUI::ImageButton>(load_image("../assets/toggleUnitButton.png"));
    button->set_toggleable(true);
    button->on_change = [this](bool state) {
        auto vel = this->m_velocity_control->value();
        auto ap = m_apogee_control->value();
        auto pe = m_perigee_control->value();
        auto dir = m_direction_xz_control->value();
        auto angle = m_orbit_angle_control->value();
        auto tilt = m_orbit_tilt_control->value();

        if (state) {
            this->m_velocity_control->set_unit("km/h");
            this->m_velocity_control->set_value(vel * 3.6);
            this->m_apogee_control->set_unit("AU");
            this->m_apogee_control->set_value(ap / AU * 1000);
            this->m_apogee_control->slider().set_range(1, 50, 0.1);
            this->m_perigee_control->set_unit("AU");
            this->m_perigee_control->set_value(pe / AU * 1000);
            this->m_perigee_control->slider().set_range(1, 50, 0.1);

            this->m_direction_xz_control->set_unit("[rad]");
            this->m_direction_xz_control->set_value(dir / 180 * M_PI);
            this->m_direction_xz_control->slider().set_range(0, 2 * M_PI, 0.01);
            this->m_orbit_angle_control->set_unit("[rad]");
            this->m_orbit_angle_control->set_value(angle / 180 * M_PI);
            this->m_orbit_angle_control->slider().set_range(0, 2 * M_PI, 0.01);
            this->m_orbit_tilt_control->set_unit("[rad]");
            this->m_orbit_tilt_control->set_value(tilt / 180 * M_PI);
            this->m_orbit_tilt_control->slider().set_range(0, 2 * M_PI, 0.01);
        }else {
            this->m_velocity_control->set_unit("m/s");
            this->m_velocity_control->set_value(vel * (1.f / 3.6));
            this->m_apogee_control->set_unit("km");
            this->m_apogee_control->set_value(ap / 1000 * AU);
            this->m_apogee_control->slider().set_range(0, 1e10, 100);
            this->m_perigee_control->set_unit("km");
            this->m_perigee_control->set_value(pe / 1000 * AU);
            this->m_perigee_control->slider().set_range(0, 1e10, 100);

            this->m_direction_xz_control->set_unit("[deg]");
            this->m_direction_xz_control->set_value(dir / M_PI * 180);
            this->m_direction_xz_control->slider().set_range(0, 360, 1);
            this->m_orbit_angle_control->set_unit("[deg]");
            this->m_orbit_angle_control->set_value(dir / M_PI * 180);
            this->m_orbit_angle_control->slider().set_range(0, 360, 1);
            this->m_orbit_tilt_control->set_unit("[deg]");
            this->m_orbit_tilt_control->set_value(dir / M_PI * 180);
            this->m_orbit_tilt_control->slider().set_range(0, 360, 1);
        }
    };
    button->set_tooltip_text("Toggle units");

    return button;
}

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

    m_direction_xz_control = container->add_widget<GUI::ValueSlider>(0, 360, 0.1);
    m_direction_xz_control->set_name("Direction X");
    m_direction_xz_control->set_unit("[deg]");
    m_direction_xz_control->on_change = [this](double) {
        m_forward_simulation_is_valid = false;
    };

    m_direction_yz_control = container->add_widget<GUI::ValueSlider>(-90, 90, 0.1);
    m_direction_yz_control->set_name("Direction Y");
    m_direction_yz_control->set_unit("[deg]");
    m_direction_yz_control->on_change = [this](double) {
        m_forward_simulation_is_valid = false;
    };

    m_y_position_control = container->add_widget<GUI::ValueSlider>(-50 * AU, 50 * AU);
    m_y_position_control->set_name("Y position");
    m_y_position_control->set_unit("AU");
    m_y_position_control->on_change = [this](double) {
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
    m_apogee_control->on_change = [this](double) {
        m_forward_simulation_is_valid = false;
    };

    m_perigee_control = container->add_widget<GUI::ValueSlider>(0, 1e10, 100);
    m_perigee_control->set_name("Perigee");
    m_perigee_control->set_unit("km");
    m_perigee_control->on_change = [this](double) {
        m_forward_simulation_is_valid = false;
    };

    m_orbit_angle_control = container->add_widget<GUI::ValueSlider>(0, 360, 1);
    m_orbit_angle_control->set_name("Angle");
    m_orbit_angle_control->set_unit("[deg]");
    m_orbit_angle_control->on_change = [this](double) {
        m_forward_simulation_is_valid = false;
    };

    m_orbit_tilt_control = container->add_widget<GUI::ValueSlider>(0, 360, 1);
    m_orbit_tilt_control->set_name("Tilt");
    m_orbit_tilt_control->set_unit("[deg]");
    m_orbit_tilt_control->on_change = [this](double) {
        m_forward_simulation_is_valid = false;
    };

    return container;
}

std::unique_ptr<Object> EssaGUI::m_create_object_from_params() const {
    double mass = std::stod(m_mass_textbox->get_content().toAnsiString()) * std::pow(10, std::stod(m_mass_exponent_textbox->get_content().toAnsiString()));
    double radius = m_radius_control->value() * 1000;
    double theta = m_direction_xz_control->value();
    double alpha = m_direction_yz_control->value();
    double velocity = m_velocity_control->value();

    Vector3 vel(std::cos(theta) * std::cos(alpha) * velocity, std::sin(theta) * std::cos(alpha) * velocity, std::sin(alpha) * velocity);

    auto pos = m_new_object_pos;
    pos.z = m_y_position_control->value();

    sf::Color color = m_color_control->value();

    std::string name = m_name_textbox->get_content();

    return std::make_unique<Object>(m_world, mass, radius, pos, vel, color, name, 1000);
}

std::unique_ptr<Object> EssaGUI::m_create_object_from_orbit() const {
    if (!m_focused)
        return nullptr;
    double mass = std::stod(m_mass_textbox->get_content().toAnsiString()) * std::pow(10, std::stod(m_mass_exponent_textbox->get_content().toAnsiString()));
    Distance radius = { static_cast<float>(m_radius_control->value() * 1000), Distance::Kilometer };
    Distance apogee, perigee;
    Angle angle, tilt;

    if (m_toggle_unit_button->is_active()) {
        apogee = { static_cast<float>(m_apogee_control->value() * AU), Distance::Au };
        perigee = { static_cast<float>(m_perigee_control->value() * AU), Distance::Au };

        angle = { static_cast<float>(m_orbit_angle_control->value()), Angle::Rad };
        tilt = { static_cast<float>(m_orbit_tilt_control->value()), Angle::Rad };
    }
    else {
        apogee = { static_cast<float>(m_apogee_control->value() * 1000), Distance::Kilometer };
        perigee = { static_cast<float>(m_perigee_control->value() * 1000), Distance::Kilometer };

        angle = { static_cast<float>(m_orbit_angle_control->value()), Angle::Deg };
        tilt = { static_cast<float>(m_orbit_tilt_control->value()), Angle::Deg };
    }

    // std::cout << radius << "\n";
    // std::cout << apogee << "\n";
    // std::cout << perigee << "\n";
    // std::cout << angle << "\n";
    // std::cout << tilt << "\n";

    return m_focused->create_object_relative_to(mass, radius, apogee, perigee, m_toggle_orbit_direction_button->is_active(), angle, tilt, m_color_control->value(), m_name_textbox->get_content(), 0.0_deg);
}
