#include "FocusedObjectGUI.hpp"

#include "../Object.hpp"
#include <EssaGUI/gui/MessageBox.hpp>
#include <EssaGUI/gui/TabWidget.hpp>
#include <EssaUtil/UnitDisplay.hpp>
#include <SFML/Graphics/Color.hpp>
#include <cmath>
#include <iomanip>
#include <memory>

FocusedObjectGUI::FocusedObjectGUI(GUI::WidgetTreeRoot& c, Object* o, GUI::ToolWindow* wnd, World& w)
    : GUI::Container(c)
    , m_focused(o)
    , m_world(w)
    , m_window(wnd) {
    set_layout<GUI::VerticalBoxLayout>();
    add_widget<GUI::Container>()->set_size({ Length::Auto, 10.0_px });
    set_background_color(Util::Color { 192, 192, 192, 30 });

    auto tab_widget = add_widget<GUI::TabWidget>();

    tab_widget->on_tab_switch = [&](unsigned index) {
        m_world.m_simulation_view->pause_simulation(index == 1);

        m_tab = index;
    };

    auto& info = tab_widget->add_tab("General info");
    auto& info_layout = info.set_layout<GUI::VerticalBoxLayout>();
    info_layout.set_spacing(5);
    info_layout.set_padding(10);
    m_create_info_gui(info);

    auto& modify = tab_widget->add_tab("Modify object");
    auto& modify_layout = modify.set_layout<GUI::VerticalBoxLayout>();
    modify_layout.set_spacing(5);
    modify_layout.set_padding(10);
    m_create_modify_gui(modify);

    auto& view = tab_widget->add_tab("View options");
    auto& view_layout = view.set_layout<GUI::VerticalBoxLayout>();
    view_layout.set_spacing(5);
    view_layout.set_padding(10);
    m_create_view_gui(view);
}

void FocusedObjectGUI::m_create_info_gui(GUI::Container& info) {
    auto title = info.add_widget<GUI::Textfield>();
    title->set_size({ Length::Auto, 40.0_px });
    title->set_alignment(GUI::Align::Center);
    title->set_font_size(30);
    title->set_content("General information");

    auto add_field = [&](Util::UString name, bool is_most_massive_data) -> Field {
        auto subcontainer = info.add_widget<GUI::Container>();
        subcontainer->set_size({ Length::Auto, 30.0_px });
        subcontainer->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
        subcontainer->add_widget<GUI::Textfield>()->set_content(name + ": ");
        auto value_label = subcontainer->add_widget<GUI::Textfield>();
        value_label->set_size({ 150.0_px, Length::Auto });
        auto unit_label = subcontainer->add_widget<GUI::Textfield>();
        unit_label->set_alignment(GUI::Align::CenterRight);
        unit_label->set_size({ 50.0_px, Length::Auto });

        if (is_most_massive_data)
            m_fields.push_back(subcontainer);
        return Field { .unit_textfield = unit_label, .value_textfield = value_label };
    };

    m_mass_textfield = add_field("Mass", false);
    m_radius_textfield = add_field("Radius", false);
    m_absolute_velocity_textfield = add_field("Absolute velocity", false);

    m_orbiting_title = info.add_widget<GUI::Textfield>();
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

void FocusedObjectGUI::m_create_modify_gui(GUI::Container& modify) {
    auto title = modify.add_widget<GUI::Textfield>();
    title->set_size({ Length::Auto, 40.0_px });
    title->set_alignment(GUI::Align::Center);
    title->set_font_size(30);
    title->set_content("Modify object");

    m_radius_control = modify.add_widget<GUI::ValueSlider>(0, 500000);
    m_radius_control->set_name("Radius");
    m_radius_control->set_unit("km");

    auto mass_container = modify.add_widget<GUI::Container>();
    mass_container->set_size({ Length::Auto, 30.0_px });
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
        m_mass_textbox->set_min_max_values(1, 9.9);

        auto mass_value_container = mass_container->add_widget<GUI::Container>();
        mass_value_container->set_layout<GUI::HorizontalBoxLayout>();
        {
            auto mass_exponent_textfield = mass_value_container->add_widget<GUI::Textfield>();
            mass_exponent_textfield->set_content(" * 10 ^ ");
            mass_exponent_textfield->set_alignment(GUI::Align::CenterLeft);

            m_mass_exponent_textbox = mass_value_container->add_widget<GUI::Textbox>();
            m_mass_exponent_textbox->set_limit(2);
            m_mass_exponent_textbox->set_content("1");
            m_mass_exponent_textbox->set_min_max_values(1, 40);

            auto mass_unit_textfield = mass_value_container->add_widget<GUI::Textfield>();
            mass_unit_textfield->set_content(" kg ");
            mass_unit_textfield->set_alignment(GUI::Align::Center);
        }
        mass_layout.set_multipliers({ 5.f / 3, 5.f / 3, 5.f / 9, 5.f / 9, 5.f / 9 });
    }

    m_velocity_control = modify.add_widget<GUI::ValueSlider>(0, 50000);
    m_velocity_control->set_name("Velocity");
    m_velocity_control->set_unit("m/s");

    m_direction_xz_control = modify.add_widget<GUI::ValueSlider>(0, 360, 1);
    m_direction_xz_control->set_name("Direction X");
    m_direction_xz_control->set_unit("[deg]");
    m_direction_xz_control->set_class_name("Angle");
    m_direction_xz_control->slider().set_wraparound(true);

    m_direction_yz_control = modify.add_widget<GUI::ValueSlider>(-90, 90, 1);
    m_direction_yz_control->set_name("Direction Y");
    m_direction_yz_control->set_unit("[deg]");
    m_direction_yz_control->set_class_name("Angle");
    m_direction_yz_control->slider().set_wraparound(true);

    m_y_position_control = modify.add_widget<GUI::ValueSlider>(-0.05 * AU, 0.05 * AU);
    m_y_position_control->set_name("Y position");
    m_y_position_control->set_unit("km");
    m_y_position_control->set_class_name("Dist");

    auto main_color_container = modify.add_widget<Container>();
    main_color_container->set_size({ Length::Auto, 150.0_px });
    auto& main_color_layout = main_color_container->set_layout<GUI::VerticalBoxLayout>();
    main_color_layout.set_spacing(10);
    {
        auto color_label_textfield = main_color_container->add_widget<GUI::Textfield>();
        color_label_textfield->set_content("COLOR");
        color_label_textfield->set_alignment(GUI::Align::Center);

        m_color_control = main_color_container->add_widget<GUI::ColorPicker>();
        m_color_control->set_size({ Length::Auto, 100.0_px });
    }
    auto name_container = modify.add_widget<Container>();
    name_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
    name_container->set_size({ Length::Auto, 30.0_px });
    {
        auto name_textfield = name_container->add_widget<GUI::Textfield>();
        name_textfield->set_size({ 150.0_px, Length::Auto });
        name_textfield->set_content("Name: ");
        name_textfield->set_alignment(GUI::Align::CenterLeft);

        m_name_textbox = name_container->add_widget<GUI::Textbox>();
        m_name_textbox->set_limit(20);
        m_name_textbox->set_data_type(GUI::Textbox::TEXT);
        m_name_textbox->set_content("Planet");
    }

    auto button_container = modify.add_widget<GUI::Container>();
    button_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
    button_container->set_size({ Length::Auto, 30.0_px });

    auto modify_button = button_container->add_widget<GUI::TextButton>();
    modify_button->set_content("Modify object");
    modify_button->override_button_colors().untoggleable.background = theme().positive;
    modify_button->set_alignment(GUI::Align::Center);
    modify_button->set_alignment(GUI::Align::Center);
    modify_button->on_click = [&]() {
        if (m_world.exist_object_with_name(m_name_textbox->get_content().encode()) && m_focused->name() != m_name_textbox->get_content().encode()) {
            GUI::message_box("Object with name: \"" + m_name_textbox->get_content() + "\" already exist!", "Error!", GUI::MessageBox::Buttons::Ok);
            return;
        }
        m_focused->delete_object();
        m_world.add_object(m_create_object_from_params());
        m_focused = m_world.last_object().get();
        m_window->set_title(Util::UString { m_focused->name() });
    };

    auto remove_button = button_container->add_widget<GUI::TextButton>();
    remove_button->set_content("Remove object");
    remove_button->override_button_colors().untoggleable.background = theme().negative;
    remove_button->on_click = [&]() {
        auto result = GUI::message_box(Util::UString { "Are you sure you want to delete object \"" + m_focused->name() + "\"?" }, "Delete object", GUI::MessageBox::Buttons::YesNo);
        if (result == GUI::MessageBox::ButtonRole::Yes) {
            m_focused->delete_object();
            m_focused = nullptr;
            m_window->close();
        };
    };
}

void FocusedObjectGUI::m_create_view_gui(GUI::Container& parent) {
    auto title = parent.add_widget<GUI::Textfield>();
    title->set_size({ Length::Auto, 40.0_px });
    title->set_alignment(GUI::Align::Center);
    title->set_font_size(30);
    title->set_content("Change object view");

    auto default_view_button_container = parent.add_widget<GUI::Container>();
    default_view_button_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
    default_view_button_container->set_size({ Length::Auto, 30.0_px });

    auto default_view_textfield = default_view_button_container->add_widget<GUI::Textfield>();
    default_view_textfield->set_content("Hide focused object window");
    auto default_view_button = default_view_button_container->add_widget<GUI::TextButton>();
    default_view_button->set_content("Hide");
    default_view_button->set_alignment(GUI::Align::Center);

    default_view_button->on_click = [&]() {
        m_window->close();
        m_world.m_simulation_view->set_focused_object(m_focused);
    };

    auto light_source_button_container = parent.add_widget<GUI::Container>();
    light_source_button_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
    light_source_button_container->set_size({ Length::Auto, 30.0_px });

    auto light_source_textfield = light_source_button_container->add_widget<GUI::Textfield>();
    light_source_textfield->set_content("Toggle Light source options");

    m_light_source_button = light_source_button_container->add_widget<GUI::TextButton>();
    m_light_source_button->set_content("Make a lightsource");
    m_light_source_button->set_active_content("Make a casual body");
    m_light_source_button->set_alignment(GUI::Align::Center);
    m_light_source_button->set_toggleable(true);
    m_light_source_button->set_active(m_focused == m_world.light_source());

    m_light_source_button->on_change = [&](bool state) {
        if (state)
            m_world.set_light_source(m_focused);
        else
            m_world.set_light_source(nullptr);
    };
}

std::unique_ptr<Object> FocusedObjectGUI::m_create_object_from_params() const {
    double mass = std::stod(m_mass_textbox->get_content().encode()) * std::pow(10, std::stod(m_mass_exponent_textbox->get_content().encode()));
    double radius = m_radius_control->value() * 1000;
    double theta = m_direction_xz_control->value();
    double alpha = m_direction_yz_control->value();
    double velocity = m_velocity_control->value();

    theta = theta / 180 * M_PI;
    alpha = alpha / 180 * M_PI;

    Util::Vector3d vel(std::cos(theta) * std::cos(alpha) * velocity, std::sin(theta) * std::cos(alpha) * velocity, std::sin(alpha) * velocity);

    auto pos = m_new_object_pos;
    pos.z() = m_y_position_control->value();

    Util::Color color = m_color_control->value();
    std::string name = m_name_textbox->get_content().encode();

    // FIXME: Trails should be calculated in realtime somehow
    return std::make_unique<Object>(mass, radius, pos, vel, color, name, 1000000);
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
    m_light_source_button->set_active(m_focused == m_world.light_source());

    if (m_focused == nullptr || m_tab != 0)
        return;

    update_params();
}

void FocusedObjectGUI::update_params() {
    set_visible(true);
    set_most_massive_data_visible(m_focused->most_attracting_object());

    if (m_focused->most_attracting_object())
        m_orbiting_title->set_content(Util::UString { "Orbiting around: " + m_focused->most_attracting_object()->name() });

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

    for (auto& f : m_fields)
        f->set_visible(m_focused->most_attracting_object() != nullptr);
    m_orbiting_title->set_visible(m_focused->most_attracting_object() != nullptr);

    m_radius_control->set_value(m_focused->radius() / 1000);
    m_velocity_control->set_value(m_focused->vel().length());

    m_new_object_pos.x() = m_focused->pos().x();
    m_new_object_pos.y() = m_focused->pos().y();
    m_y_position_control->set_value(m_focused->pos().z());
    m_direction_xz_control->set_value(std::atan2(m_new_object_pos.y(), m_new_object_pos.x()) / M_PI * 180 - 90);
    m_direction_yz_control->set_value(std::atan2(m_new_object_pos.y(), m_new_object_pos.z()) / M_PI * 180 - 90);

    double mass = m_focused->mass();
    m_mass_exponent_textbox->set_content(Util::UString { std::to_string(static_cast<int>(std::log10(mass))) });
    m_mass_textbox->set_content(Util::UString { std::to_string(mass / std::pow(10, (int)std::log10(mass))) });

    m_color_control->set_value(m_focused->color());
    m_name_textbox->set_content(Util::UString { m_focused->name() });
}
