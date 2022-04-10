#include "GUI.hpp"
#include "../Object.hpp"
#include "../World.hpp"
#include "Button.hpp"
#include "Container.hpp"
#include "PythonREPL.hpp"
#include "SimulationView.hpp"
#include "Textbox.hpp"
#include "Textfield.hpp"
#include "ToggleButton.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cmath>
#include <iostream>
#include <string>

sf::Font GUI::font;
sf::Font GUI::bold_font;
sf::Font GUI::fixed_width_font;

static sf::Image load_image(std::string path) {
    // TODO: Error handling
    sf::Image image;
    image.loadFromFile(path);
    return image;
}

GUI::GUI(Application& application, World& world)
    : Container(application)
    , m_world(world) {
    font.loadFromFile("../assets/fonts/Xolonium-pn4D.ttf");
    bold_font.loadFromFile("../assets/fonts/XoloniumBold-xKZO.ttf");
    fixed_width_font.loadFromFile("../assets/fonts/SourceCodePro-Regular.otf");
    set_layout<BasicLayout>();

    m_simulation_view = add_widget<SimulationView>(world);
    m_simulation_view->set_size({ { 100, Length::Percent }, { 100, Length::Percent } });
    m_simulation_view->on_coord_measure = [&](Vector3 pos) {
        // TODO: Add widget enabled state and use it instead.
        m_add_object_button->set_visible(true);
        m_forward_simulation_is_valid = false;
        m_new_object_pos = pos;
    };

    m_simulation_view->on_focus_measure = [&](Object* focusing) {
        // TODO: Add widget enabled state and use it instead.
        m_add_object_button->set_visible(true);
        m_focused = focusing;
        // std::cout << m_focused << "\n";
    };

    m_world.m_simulation_view = m_simulation_view.get();

    auto menu = add_widget<SettingsMenu>();
    menu->set_position({ 10.0_px, 10.0_px });
    {
        auto& create_menu = menu->add_entry(load_image("../assets/createButton.png"), "Create new object");
        create_menu.on_toggle = [this](bool state) {
            if (!state) {
                m_new_object = nullptr;
                m_forward_simulation_is_valid = true;
                m_simulation_view->set_speed(m_saved_speed);
            }
            else {
                // TODO: Don't allow to resume simulation.
                m_saved_speed = m_simulation_view->speed();
                m_simulation_view->set_speed(0);
            }
        };

        // TODO: Shrink-to-fit
        create_menu.settings_container->set_size({ 500.0_px, 500.0_px });
        auto& layout = create_menu.settings_container->set_layout<VerticalBoxLayout>();
        layout.set_spacing(10);
        {
            m_create_object_gui(*create_menu.settings_container);

            auto mode_specific_options_container = create_menu.settings_container->add_widget<Container>();
            mode_specific_options_container->set_size({ Length::Auto, 100.0_px });
            mode_specific_options_container->set_layout<BasicLayout>();

            m_create_object_from_params_container = m_create_object_from_params_gui(mode_specific_options_container);
            m_create_object_from_params_container->set_size({ { 100, Length::Percent }, { 100, Length::Percent } });
            m_create_object_from_params_container->set_visible(true);
            mode_specific_options_container->add_created_widget(m_create_object_from_params_container);

            m_create_object_from_orbit_container = m_create_object_from_orbit_gui(mode_specific_options_container);
            m_create_object_from_orbit_container->set_size({ { 100, Length::Percent }, { 100, Length::Percent } });
            m_create_object_from_orbit_container->set_visible(false);
            mode_specific_options_container->add_created_widget(m_create_object_from_orbit_container);

            auto main_color_container = create_menu.settings_container->add_widget<Container>();
            main_color_container->set_size({ Length::Auto, 150.0_px });
            auto& main_color_layout = main_color_container->set_layout<VerticalBoxLayout>();
            main_color_layout.set_spacing(10);
            {
                auto color_label_textfield = main_color_container->add_widget<Textfield>();
                color_label_textfield->set_font_size(20);
                color_label_textfield->set_content("COLOR");
                color_label_textfield->set_alignment(Align::Center);

                m_color_control = main_color_container->add_widget<ColorPicker>();
                m_color_control->set_size({ Length::Auto, 100.0_px });
            }
            auto name_container = create_menu.settings_container->add_widget<Container>();
            auto& name_layout = name_container->set_layout<HorizontalBoxLayout>();
            name_layout.set_spacing(10);
            {
                auto name_textfield = name_container->add_widget<Textfield>();
                name_textfield->set_size({ 150.0_px, Length::Auto });
                name_textfield->set_font_size(20);
                name_textfield->set_content("Name: ");
                name_textfield->set_alignment(Align::CenterLeft);

                m_name_textbox = name_container->add_widget<Textbox>();
                m_name_textbox->set_limit(20);
                m_name_textbox->set_data_type(Textbox::TEXT);
                m_name_textbox->set_content("Planet");
            }
            m_submit_container = create_menu.settings_container->add_widget<Container>();
            m_submit_container->set_size({ Length::Auto, 72.0_px });
            auto& submit_layout = m_submit_container->set_layout<HorizontalBoxLayout>();
            submit_layout.set_spacing(10);
            {
                m_coords_button = m_submit_container->add_widget<Button>(load_image("../assets/coordsButton.png"));
                m_coords_button->on_click = [this]() {
                    if (m_automatic_orbit_calculation)
                        m_simulation_view->start_focus_measure();
                    else
                        m_simulation_view->start_coords_measure();
                };
                m_coords_button->set_tooltip_text("Set position");

                m_toggle_unit_button = m_submit_container->add_widget<ToggleButton>(load_image("../assets/toggleUnitButton.png"));
                m_toggle_unit_button->on_change = [this](bool state) {
                    this->m_units = state;
                    auto vel = this->m_velocity_control->value();
                    if (state) {
                        this->m_velocity_control->set_unit("km/h");
                        this->m_velocity_control->set_value(vel * 3.6);
                    }
                    else {
                        this->m_velocity_control->set_unit("m/s");
                        this->m_velocity_control->set_value(vel * (1.f / 3.6));
                    }
                };
                m_toggle_unit_button->set_tooltip_text("Toggle units");

                m_creative_mode_button = m_submit_container->add_widget<ToggleButton>(load_image("../assets/toggleCreativeModeButton.png"));
                m_creative_mode_button->set_tooltip_text("Toggle automatic orbit calculation");

                m_toggle_orbit_direction_button = m_submit_container->add_widget<ToggleButton>(load_image("../assets/orbitDirectionButton.png"));
                m_toggle_orbit_direction_button->set_tooltip_text("Toggle orbitting body direction");
                m_toggle_orbit_direction_button->on_change = [](bool state) {
                };

                m_creative_mode_button->on_change = [this](bool state) {
                    m_create_object_from_params_container->set_visible(!state);
                    m_create_object_from_orbit_container->set_visible(state);
                    m_toggle_orbit_direction_button->set_visible(state);
                    m_automatic_orbit_calculation = state;
                };
                m_toggle_orbit_direction_button->set_visible(false);

                m_submit_container->add_widget<Widget>(); // spacer

                m_add_object_button = m_submit_container->add_widget<Button>(load_image("../assets/addObjectButton.png"));
                m_add_object_button->on_click = [&world, this]() {
                    // FIXME: This (object_list) should be probably private.
                    world.add_object(m_create_object_from_params());
                    m_simulation_view->m_measured = false;
                };
                m_add_object_button->set_tooltip_text("Add object");
            }
        }
    }
    {
        auto& settings = menu->add_entry(load_image("../assets/simulationSettings.png"), "Simulation Settings");
        create_simulation_settings_gui(*settings.settings_container);
    }

    auto python_repl = add_widget<PythonREPL>();
    python_repl->set_position({ 300.0_px, 10.0_px_o });
    python_repl->set_size({ 700.0_px, 250.0_px });

    m_home_button = add_widget<Button>(load_image("../assets/homeButton.png"));
    m_home_button->set_position({ 10.0_px_o, 10.0_px_o });
    m_home_button->on_click = [this]() {
        m_simulation_view->set_offset(sf::Vector2f(0, 0));
        m_simulation_view->set_zoom(1);
        m_simulation_view->reset_rotation();
    };
    m_home_button->set_tooltip_text("Reset coordinates");
}

void GUI::create_simulation_settings_gui(Container& container) {

    container.set_size({ 500.0_px, 500.0_px });
    container.set_layout<VerticalBoxLayout>().set_spacing(10);
    auto label = container.add_widget<Textfield>();
    label->set_content("Simulation Settings");
    label->set_size({ Length::Auto, 30.0_px });

    auto iterations_control = container.add_widget<ValueSlider>(1, 1000);
    iterations_control->set_name("Iterations");
    iterations_control->set_unit("i/t");
    iterations_control->set_value(10);
    iterations_control->set_tooltip_text("Count of simulation ticks per render frame");
    iterations_control->on_change = [this](double value) {
        if (value > 0)
            m_simulation_view->set_iterations(value);
    };

    auto tick_length_control = container.add_widget<ValueSlider>(60, 60 * 60 * 24, 60);
    tick_length_control->set_name("Tick Length");
    tick_length_control->set_unit("s/t");
    tick_length_control->set_value(60 * 60 * 12); // 12h / half a day
    tick_length_control->set_tooltip_text("Amount of simulation seconds per simulation tick (Affects accuracy)");
    tick_length_control->on_change = [this](double value) {
        if (value > 0)
            m_world.set_simulation_seconds_per_tick(value);
    };

    auto toggle_labels_container = container.add_widget<Container>();
    auto& toggle_labels_layout = toggle_labels_container->set_layout<HorizontalBoxLayout>();
    toggle_labels_layout.set_spacing(10);
    toggle_labels_container->set_size({ Length::Auto, 30.0_px });

    auto button_label = toggle_labels_container->add_widget<Textfield>();
    button_label->set_content("Toggle labels: ");
    button_label->set_size({ Length::Auto, 30.0_px });
    auto toggle_labels = toggle_labels_container->add_widget<TextButton>();
    toggle_labels->set_content("Off");
    toggle_labels->set_active_content("On");
    toggle_labels->set_display_attributes(sf::Color::Green, sf::Color(0, 150, 0), sf::Color::White);
    toggle_labels->set_active_display_attributes(sf::Color::Red, sf::Color(150, 0, 0), sf::Color::White);
    toggle_labels->set_active(true);
    toggle_labels->on_change = [this](bool state){
        this->m_simulation_view->toggle_label_visibility();
    };
    
}

void GUI::recalculate_forward_simulation() {
    auto new_object = m_create_object_from_params();

    m_world.clone_for_forward_simulation(m_forward_simulated_world);
    auto forward_simulated_new_object = new_object.get();

    // We need trail of the forward simulated object but
    // the object itself will be drawn at current position.
    m_new_object = new_object->clone_for_forward_simulation(m_forward_simulated_world);
    m_forward_simulated_world.add_object(std::move(new_object));

    m_forward_simulated_world.update(500);

    // FIXME: bruh trail invalidation
    forward_simulated_new_object->trail().push_front(m_new_object->m_pos);
    forward_simulated_new_object->trail().update_trail(*m_simulation_view, forward_simulated_new_object->m_color);

    m_forward_simulation_is_valid = true;
}

void GUI::relayout() {
    set_raw_size(sf::Vector2f(window().getSize()));
    Container::relayout();
}

void GUI::update() {
    // FIXME: We should do this on every tick or just stop the main simulation.
    if (!m_forward_simulation_is_valid)
        recalculate_forward_simulation();
}

void GUI::draw(sf::RenderWindow& window) const {
    if (m_new_object) {
        m_forward_simulated_world.draw(*m_simulation_view);
        WorldDrawScope scope(*m_simulation_view);
        m_new_object->draw(*m_simulation_view);
    }
}

void GUI::handle_event(Event& event) {
    if (event.type() == sf::Event::Closed) {
        window().close();
        event.set_handled();
    }
}
