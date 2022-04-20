#include "FocusedObjectGUI.hpp"

#include "Object.hpp"
#include <cmath>
#include <iomanip>

FocusedObjectGUI::FocusedObjectGUI(GUI::Container& c)
    : GUI::Container(c) {
    auto& layout = set_layout<GUI::VerticalBoxLayout>();
    layout.set_spacing(5);

    m_title = add_widget<GUI::Textfield>().get();
    m_title->set_size({ Length::Auto, 45.0_px });
    m_title->set_alignment(GUI::Align::Center);
    m_title->set_font_size(30);

    auto add_field = [&](std::string name, std::string unit, bool is_most_massive_data) {
        auto subcontainer = add_widget<GUI::Container>();
        subcontainer->set_size({ Length::Auto, 30.0_px });
        if (is_most_massive_data)
            subcontainer->set_class_name("most_massive_data");
        subcontainer->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
        subcontainer->add_widget<GUI::Textfield>()->set_content(name + ": ");
        auto value_label = subcontainer->add_widget<GUI::Textfield>();
        value_label->set_size({ 150.0_px, Length::Auto });
        auto unit_label = subcontainer->add_widget<GUI::Textfield>();
        unit_label->set_content(unit);
        unit_label->set_alignment(GUI::Align::CenterRight);
        unit_label->set_size({ 50.0_px, Length::Auto });
        return value_label.get();
    };

    m_mass_textfield = add_field("Mass", "kg", false);
    m_radius_textfield = add_field("Radius", "m", false);
    m_absolute_velocity_textfield = add_field("Absolute velocity", "m/s", false);

    m_orbiting_title = add_widget<GUI::Textfield>().get();
    m_orbiting_title->set_size({ Length::Auto, 35.0_px });
    m_orbiting_title->set_alignment(GUI::Align::Center);
    m_orbiting_title->set_font_size(20);
    m_orbiting_title->set_class_name("most_massive_data");

    m_distance_from_most_massive_object_textfield = add_field("Distance", "AU", true);
    m_apogee_textfield = add_field("Apogee", "AU", true);
    m_apogee_velocity_textfield = add_field("Apogee velocity", "m/s", true);
    m_perigee_textfield = add_field("Perigee", "AU", true);
    m_perigee_velocity_textfield = add_field("Perigee velocity", "m/s", true);
    m_orbit_period_textfield = add_field("Orbit period", "years", true);
    m_orbit_eccencrity_textfield = add_field("Orbit eccencrity", "", true);
}

void FocusedObjectGUI::set_most_massive_data_visible(bool visible) {
    if (visible)
        set_size({ 380.0_px, 345.0_px });
    else
        set_size({ 380.0_px, 135.0_px });
    auto object_infos = find_widgets_by_class_name("most_massive_data");

    for (auto& info : object_infos)
        info->set_visible(visible);
}

void FocusedObjectGUI::update_from_object(Object* focused) {
    if (!focused) {
        set_visible(false);
        return;
    }
    set_visible(true);
    set_most_massive_data_visible(focused->most_attracting_object());

    m_title->set_content(focused->name());
    if (focused->most_attracting_object())
        m_orbiting_title->set_content("Orbiting around: " + focused->most_attracting_object()->name());

    auto info = focused->get_info();

    auto to_exponent_string = [](double value) -> sf::String {
        std::ostringstream oss;
        if (value < 10) {
            oss << value;
            return oss.str();
        }
        if (value < 10'000'000) {
            oss << std::fixed << std::setprecision(2) << value;
            return oss.str();
        }
        int exponent = (double)std::log10(value);
        double mantissa = value / std::pow(10, exponent);
        oss << std::fixed << std::setprecision(4) << mantissa << " \u00D7 10^" << exponent; // × (MULTIPLICATION SIGN)
        auto oss_str = oss.str();
        return sf::String::fromUtf8(oss_str.begin(), oss_str.end());
    };

    m_mass_textfield->set_content(to_exponent_string(info.mass));
    m_radius_textfield->set_content(to_exponent_string(info.radius));
    m_absolute_velocity_textfield->set_content(to_exponent_string(info.absolute_velocity));
    m_distance_from_most_massive_object_textfield->set_content(to_exponent_string(info.distance_from_most_massive_object));
    m_apogee_textfield->set_content(to_exponent_string(info.apogee));
    m_apogee_velocity_textfield->set_content(to_exponent_string(info.apogee_velocity));
    m_perigee_textfield->set_content(to_exponent_string(info.perigee));
    m_perigee_velocity_textfield->set_content(to_exponent_string(info.perigee_velocity));
    m_orbit_period_textfield->set_content(to_exponent_string(info.orbit_period));
    m_orbit_eccencrity_textfield->set_content(to_exponent_string(info.orbit_eccencrity));
}
