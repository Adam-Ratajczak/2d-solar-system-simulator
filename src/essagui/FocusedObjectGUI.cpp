#include "FocusedObjectGUI.hpp"

#include "../Object.hpp"
#include "../util/UnitDisplay.hpp"
#include <cmath>
#include <iomanip>

FocusedObjectGUI::FocusedObjectGUI(GUI::WidgetTreeRoot& c, Object* o)
    : GUI::Container(c)
    , m_focused(o) {
    auto& layout = set_layout<GUI::VerticalBoxLayout>();
    layout.set_spacing(5);

    m_title = add_widget<GUI::Textfield>();
    m_title->set_size({ Length::Auto, 45.0_px });
    m_title->set_alignment(GUI::Align::Center);
    m_title->set_font_size(30);

    auto add_field = [&](std::string name, bool is_most_massive_data) -> Field {
        auto subcontainer = add_widget<GUI::Container>();
        subcontainer->set_size({ Length::Auto, 30.0_px });
        if (is_most_massive_data)
            subcontainer->set_class_name("most_massive_data");
        subcontainer->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
        subcontainer->add_widget<GUI::Textfield>()->set_content(name + ": ");
        auto value_label = subcontainer->add_widget<GUI::Textfield>();
        value_label->set_size({ 150.0_px, Length::Auto });
        auto unit_label = subcontainer->add_widget<GUI::Textfield>();
        unit_label->set_alignment(GUI::Align::CenterRight);
        unit_label->set_size({ 50.0_px, Length::Auto });
        return Field { .unit_textfield = unit_label, .value_textfield = value_label };
    };

    m_mass_textfield = add_field("Mass", false);
    m_radius_textfield = add_field("Radius", false);
    m_absolute_velocity_textfield = add_field("Absolute velocity", false);

    m_orbiting_title = add_widget<GUI::Textfield>();
    m_orbiting_title->set_size({ Length::Auto, 35.0_px });
    m_orbiting_title->set_alignment(GUI::Align::Center);
    m_orbiting_title->set_font_size(20);
    m_orbiting_title->set_class_name("most_massive_data");

    m_distance_from_most_massive_object_textfield = add_field("Distance", true);
    m_apogee_textfield = add_field("Apogee", true);
    m_apogee_velocity_textfield = add_field("Apogee velocity", true);
    m_perigee_textfield = add_field("Perigee", true);
    m_perigee_velocity_textfield = add_field("Perigee velocity", true);
    m_orbit_period_textfield = add_field("Orbit period", true);
    m_orbit_eccencrity_textfield = add_field("Orbit eccencrity", true);
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

void FocusedObjectGUI::Field::set_content_from_unit_value(Util::UnitValue const& value) const {
    unit_textfield->set_content(value.unit);
    value_textfield->set_content(value.value);
}

void FocusedObjectGUI::update() {
    set_visible(true);
    set_most_massive_data_visible(m_focused->most_attracting_object());

    m_title->set_content(m_focused->name());
    if (m_focused->most_attracting_object())
        m_orbiting_title->set_content("Orbiting around: " + m_focused->most_attracting_object()->name());

    auto info = m_focused->get_info();

    m_mass_textfield.set_content_from_unit_value(Util::unit_display(info.mass, Util::Quantity::Mass));
    m_radius_textfield.set_content_from_unit_value(Util::unit_display(info.radius, Util::Quantity::Length));
    m_absolute_velocity_textfield.set_content_from_unit_value(Util::unit_display(info.absolute_velocity, Util::Quantity::Velocity));
    m_distance_from_most_massive_object_textfield.set_content_from_unit_value(Util::unit_display(info.distance_from_most_massive_object, Util::Quantity::Length));
    m_apogee_textfield.set_content_from_unit_value(Util::unit_display(info.apogee, Util::Quantity::Length));
    m_apogee_velocity_textfield.set_content_from_unit_value(Util::unit_display(info.apogee_velocity, Util::Quantity::Velocity));
    m_perigee_textfield.set_content_from_unit_value(Util::unit_display(info.perigee, Util::Quantity::Length));
    m_perigee_velocity_textfield.set_content_from_unit_value(Util::unit_display(info.perigee_velocity, Util::Quantity::Velocity));
    m_orbit_period_textfield.set_content_from_unit_value(Util::unit_display(info.orbit_period, Util::Quantity::Time));
    m_orbit_eccencrity_textfield.set_content_from_unit_value(Util::unit_display(info.orbit_eccencrity, Util::Quantity::None));
}
