#include "GUI.hpp"

void GUI::m_create_object_from_params_gui(std::shared_ptr<Container> container) {
    auto direction_container = container->add_widget<Container>();
    {
        auto& direction_layout = direction_container->set_layout<HorizontalBoxLayout>();
        direction_layout.set_spacing(10);

        auto direction_textfield = direction_container->add_widget<Textfield>();
        direction_textfield->set_size({ 150.0_px, Length::Auto });
        direction_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
        direction_textfield->set_font_size(20);
        direction_textfield->set_content("Direction: ");
        direction_textfield->set_alignment(Textfield::Align::CenterLeft);

        m_direction_slider = direction_container->add_widget<Slider>(0, 360, 1);
        m_direction_slider->set_display_attributes(sf::Color(200, 200, 200), sf::Color(255, 255, 255));

        auto direction_unit_textfield = direction_container->add_widget<Textfield>();
        direction_unit_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
        direction_unit_textfield->set_font_size(20);
        direction_unit_textfield->set_content(std::to_string((int)m_direction_slider->get_value()) + " [deg]");
        direction_unit_textfield->set_alignment(Textfield::Align::Center);

        m_direction_slider->on_change = [direction_unit_textfield](double value) {
            direction_unit_textfield->set_content(std::to_string((int)value) + " [deg]");
        };
    }
}

Object GUI::m_create_object_from_params() const {
    double mass = std::stod(m_mass_textbox->get_content().toAnsiString()) * std::pow(10, std::stod(m_mass_exponent_textbox->get_content().toAnsiString()));
    double radius = m_radius_control->value() * 1000;

    double theta = m_direction_slider->get_value() / 360 * 2 * M_PI;
    double velocity = m_velocity_control->value();
    Vector2 vel(std::cos(theta) * velocity, std::sin(theta) * velocity);

    sf::Color color(
        m_color_r_slider->get_value(),
        m_color_g_slider->get_value(),
        m_color_b_slider->get_value());

    std::string name = m_name_textbox->get_content();

    return Object(m_world, mass, radius, m_new_object_pos, vel, color, name, 1000);
}
