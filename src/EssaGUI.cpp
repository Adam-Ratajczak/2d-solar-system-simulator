#include "EssaGUI.hpp"

#include "Object.hpp"
#include "SimulationView.hpp"
#include "World.hpp"
#include "glwrapper/Sphere.hpp"
#include "gui/Button.hpp"
#include "gui/Container.hpp"
#include "gui/ImageButton.hpp"
#include "gui/PythonREPL.hpp"
#include "gui/StateTextButton.hpp"
#include "gui/Textbox.hpp"
#include "gui/Textfield.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>

static sf::Image load_image(std::string path) {
    // TODO: Error handling
    sf::Image image;
    image.loadFromFile(path);
    return image;
}

EssaGUI::EssaGUI(GUI::Application& application, World& world)
    : Container(application)
    , m_world(world) {
    set_layout<GUI::BasicLayout>();

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
    m_focused_object_info = m_create_focused_object_info_gui();
    m_focused_object_info->set_visible(false);

    m_world.m_simulation_view = m_simulation_view.get();

    auto menu = add_widget<GUI::SettingsMenu>();
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
        create_menu.settings_container->set_size({ 500.0_px, 590.0_px });
        auto& layout = create_menu.settings_container->set_layout<GUI::VerticalBoxLayout>();
        layout.set_spacing(10);
        {
            m_create_object_gui(*create_menu.settings_container);

            auto mode_specific_options_container = create_menu.settings_container->add_widget<Container>();
            mode_specific_options_container->set_size({ Length::Auto, 150.0_px });
            mode_specific_options_container->set_layout<GUI::BasicLayout>();

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
            auto& main_color_layout = main_color_container->set_layout<GUI::VerticalBoxLayout>();
            main_color_layout.set_spacing(10);
            {
                auto color_label_textfield = main_color_container->add_widget<GUI::Textfield>();
                color_label_textfield->set_content("COLOR");
                color_label_textfield->set_alignment(GUI::Align::Center);

                m_color_control = main_color_container->add_widget<GUI::ColorPicker>();
                m_color_control->set_size({ Length::Auto, 100.0_px });
            }
            auto name_container = create_menu.settings_container->add_widget<Container>();
            auto& name_layout = name_container->set_layout<GUI::HorizontalBoxLayout>();
            name_layout.set_spacing(10);
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
            m_submit_container = create_menu.settings_container->add_widget<Container>();
            m_submit_container->set_size({ Length::Auto, 72.0_px });
            auto& submit_layout = m_submit_container->set_layout<GUI::HorizontalBoxLayout>();
            submit_layout.set_spacing(10);
            {
                m_coords_button = m_submit_container->add_widget<GUI::ImageButton>(load_image("../assets/coordsButton.png"));
                m_coords_button->on_click = [this]() {
                    if (m_automatic_orbit_calculation)
                        m_simulation_view->start_focus_measure();
                    else
                        m_simulation_view->start_coords_measure();
                };
                m_coords_button->set_tooltip_text("Set position");

                m_toggle_unit_button = m_submit_container->add_widget<GUI::ImageButton>(load_image("../assets/toggleUnitButton.png"));
                m_toggle_unit_button->set_toggleable(true);
                m_toggle_unit_button->on_change = [this](bool state) {
                    this->m_units = state;
                    auto vel = this->m_velocity_control->value();
                    auto ap = m_apogee_control->value();
                    auto pe = m_perigee_control->value();
                    auto dir = m_direction_control->value();
                    auto angle = m_orbit_angle_control->value();
                    auto tilt = m_orbiit_tilt_control->value();

                    if (state) {
                        this->m_velocity_control->set_unit("km/h");
                        this->m_velocity_control->set_value(vel * 3.6);
                        this->m_apogee_control->set_unit("AU");
                        this->m_apogee_control->set_value(ap / AU * 1000);
                        this->m_apogee_control->slider().set_range(1, 50, 0.1);
                        this->m_perigee_control->set_unit("AU");
                        this->m_perigee_control->set_value(pe / AU * 1000);
                        this->m_perigee_control->slider().set_range(1, 50, 0.1);

                        this->m_direction_control->set_unit("[rad]");
                        this->m_direction_control->set_value(dir / 180 * M_PI);
                        this->m_direction_control->slider().set_range(0, 2 * M_PI, 0.01);
                        this->m_orbit_angle_control->set_unit("[rad]");
                        this->m_orbit_angle_control->set_value(angle / 180 * M_PI);
                        this->m_orbit_angle_control->slider().set_range(0, 2 * M_PI, 0.01);
                        this->m_orbiit_tilt_control->set_unit("[rad]");
                        this->m_orbiit_tilt_control->set_value(tilt / 180 * M_PI);
                        this->m_orbiit_tilt_control->slider().set_range(0, 2 * M_PI, 0.01);
                    }else {
                        this->m_velocity_control->set_unit("m/s");
                        this->m_velocity_control->set_value(vel * (1.f / 3.6));
                        this->m_apogee_control->set_unit("km");
                        this->m_apogee_control->set_value(ap / 1000 * AU);
                        this->m_apogee_control->slider().set_range(0, 1e10, 100);
                        this->m_perigee_control->set_unit("km");
                        this->m_perigee_control->set_value(pe / 1000 * AU);
                        this->m_perigee_control->slider().set_range(0, 1e10, 100);

                        this->m_direction_control->set_unit("[deg]");
                        this->m_direction_control->set_value(dir / M_PI * 180);
                        this->m_direction_control->slider().set_range(0, 360, 1);
                        this->m_orbit_angle_control->set_unit("[deg]");
                        this->m_orbit_angle_control->set_value(dir / M_PI * 180);
                        this->m_orbit_angle_control->slider().set_range(0, 360, 1);
                        this->m_orbiit_tilt_control->set_unit("[deg]");
                        this->m_orbiit_tilt_control->set_value(dir / M_PI * 180);
                        this->m_orbiit_tilt_control->slider().set_range(0, 360, 1);
                    }
                };
                m_toggle_unit_button->set_tooltip_text("Toggle units");

                m_creative_mode_button = m_submit_container->add_widget<GUI::ImageButton>(load_image("../assets/toggleCreativeModeButton.png"));
                m_creative_mode_button->set_toggleable(true);
                m_creative_mode_button->set_tooltip_text("Toggle automatic orbit calculation");

                m_toggle_orbit_direction_button = m_submit_container->add_widget<GUI::ImageButton>(load_image("../assets/orbitDirectionButton.png"));
                m_toggle_orbit_direction_button->set_toggleable(true);
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

                m_add_object_button = m_submit_container->add_widget<GUI::ImageButton>(load_image("../assets/addObjectButton.png"));
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

    auto python_repl = add_widget<GUI::PythonREPL>();
    python_repl->set_position({ 300.0_px, 10.0_px_o });
    python_repl->set_size({ 700.0_px, 250.0_px });

    m_home_button = add_widget<GUI::ImageButton>(load_image("../assets/homeButton.png"));
    m_home_button->set_position({ 10.0_px_o, 10.0_px_o });
    m_home_button->on_click = [this]() {
        m_simulation_view->set_offset(sf::Vector2f(0, 0));
        m_simulation_view->set_zoom(1);
        m_simulation_view->reset_rotation();
    };
    m_home_button->set_tooltip_text("Reset coordinates");
}

void EssaGUI::create_simulation_settings_gui(Container& container) {

    container.set_size({ 500.0_px, 400.0_px });
    container.set_layout<GUI::VerticalBoxLayout>().set_spacing(10);
    auto label = container.add_widget<GUI::Textfield>();
    label->set_content("Simulation Settings");
    label->set_size({ Length::Auto, 30.0_px });
    label->set_alignment(GUI::Align::Center);

    auto iterations_control = container.add_widget<GUI::ValueSlider>(1, 1000);
    iterations_control->set_name("Iterations");
    iterations_control->set_unit("i/t");
    iterations_control->set_value(10);
    iterations_control->set_tooltip_text("Count of simulation ticks per render frame");
    iterations_control->on_change = [this](double value) {
        if (value > 0)
            m_simulation_view->set_iterations(value);
    };

    auto tick_length_control = container.add_widget<GUI::ValueSlider>(60, 60 * 60 * 24, 60);
    tick_length_control->set_name("Tick Length");
    tick_length_control->set_unit("s/t");
    tick_length_control->set_value(60 * 60 * 12); // 12h / half a day
    tick_length_control->set_tooltip_text("Amount of simulation seconds per simulation tick (Affects accuracy)");
    tick_length_control->on_change = [this](double value) {
        if (value > 0)
            m_world.set_simulation_seconds_per_tick(value);
    };

    auto toggle_sphere_mode_container = container.add_widget<GUI::Container>();
    auto& toggle_sphere_mode_layout = toggle_sphere_mode_container->set_layout<GUI::HorizontalBoxLayout>();
    toggle_sphere_mode_layout.set_spacing(10);
    toggle_sphere_mode_container->set_size({ Length::Auto, 30.0_px });

    auto sphere_mode_label = toggle_sphere_mode_container->add_widget<GUI::Textfield>();
    sphere_mode_label->set_content("Toggle Sphere mode: ");

    auto toggle_sphere_mode = toggle_sphere_mode_container->add_widget<GUI::StateTextButton<Sphere::DrawMode>>();
    toggle_sphere_mode->add_state("Full Sphere", Sphere::DrawMode::Full, sf::Color::Green);
    toggle_sphere_mode->add_state("Grid Sphere", Sphere::DrawMode::Grid, sf::Color::Blue);
    toggle_sphere_mode->add_state("Fancy Sphere", Sphere::DrawMode::Full, sf::Color::Red);
    toggle_sphere_mode->set_alignment(GUI::Align::Center);
    toggle_sphere_mode->on_change = [this](Sphere::DrawMode mode) {
        this->m_world.for_each_object([mode](Object& object) {
            object.sphere().set_draw_mode(mode);
        });
    };

    auto add_toggle = [&](std::string title, auto on_change) {
        auto toggle_container = container.add_widget<GUI::Container>();
        auto& toggle_layout = toggle_container->set_layout<GUI::HorizontalBoxLayout>();
        toggle_layout.set_spacing(10);
        toggle_container->set_size({ Length::Auto, 30.0_px });
        auto button_label = toggle_container->add_widget<GUI::Textfield>();
        button_label->set_content(title + ": ");
        button_label->set_size({ Length::Auto, 30.0_px });
        auto toggle = toggle_container->add_widget<GUI::TextButton>();
        toggle->set_content("Off");
        toggle->set_active_content("On");
        toggle->set_toggleable(true);
        toggle->set_active(true);
        toggle->set_alignment(GUI::Align::Center);
        toggle->on_change = std::move(on_change);
        return toggle;
    };

    auto show_labels_toggle = add_toggle("Show labels", [this](bool state) {
        this->m_simulation_view->toggle_label_visibility(state);
    });
    auto show_grid_toggle = add_toggle("Show grid", [this](bool state) {
        this->m_simulation_view->set_show_grid(state);
    });
    auto show_trails_toggle = add_toggle("Show trails", [this](bool state) {
        this->m_simulation_view->set_show_trails(state);
    });

    container.add_widget<GUI::Widget>(); // SPACER

    auto restore_sim_container = container.add_widget<GUI::Container>();
    auto& restore_sim_layout = restore_sim_container->set_layout<GUI::HorizontalBoxLayout>();
    restore_sim_layout.set_spacing(10);
    restore_sim_container->set_size({ Length::Auto, 30.0_px });

    auto restore_sim_label = restore_sim_container->add_widget<GUI::Textfield>();
    restore_sim_label->set_content("Restore Simulation state: ");
    auto restore_sim = restore_sim_container->add_widget<GUI::TextButton>();
    restore_sim->set_content("Restore");
    restore_sim->set_toggleable(false);
    restore_sim->set_alignment(GUI::Align::Center);
    restore_sim->set_display_attributes(sf::Color::Green, sf::Color::Green, sf::Color::White);
    restore_sim->on_click = [world = &m_world]() {
        world->reset();
    };

    auto restore_defaults_container = container.add_widget<GUI::Container>();
    auto& restore_defaults_layout = restore_defaults_container->set_layout<GUI::HorizontalBoxLayout>();
    restore_defaults_layout.set_spacing(10);
    restore_defaults_container->set_size({ Length::Auto, 30.0_px });

    auto restore_defaults_label = restore_defaults_container->add_widget<GUI::Textfield>();
    restore_defaults_label->set_content("Restore defaults: ");
    auto restore_defaults = restore_defaults_container->add_widget<GUI::TextButton>();
    restore_defaults->set_content("Restore");
    restore_defaults->set_toggleable(false);
    restore_defaults->set_alignment(GUI::Align::Center);
    restore_defaults->set_display_attributes(sf::Color::Blue, sf::Color::Blue, sf::Color::White);
    restore_defaults->on_click = [this, iterations_control, tick_length_control, show_labels_toggle, show_grid_toggle, show_trails_toggle, toggle_sphere_mode]() {
        iterations_control->set_value(10);
        tick_length_control->set_value(60 * 60 * 12);
        show_labels_toggle->set_active(true);
        show_grid_toggle->set_active(true);
        show_trails_toggle->set_active(true);
        this->m_simulation_view->toggle_label_visibility(true);
        toggle_sphere_mode->set_index(0);
    };
}

void EssaGUI::recalculate_forward_simulation() {
    std::unique_ptr<Object> new_object;

    // FIXME: The object should be created only once and its parameters adjusted
    //        to new settings.
    if (m_automatic_orbit_calculation)
        new_object = m_create_object_from_orbit();
    else
        new_object = m_create_object_from_params();

    if (!new_object)
        return;

    m_world.clone_for_forward_simulation(m_forward_simulated_world);
    auto forward_simulated_new_object = new_object.get();

    // We need trail of the forward simulated object but
    // the object itself will be drawn at current position.
    m_new_object = new_object->clone_for_forward_simulation(m_forward_simulated_world);
    m_forward_simulated_world.add_object(std::move(new_object));

    m_forward_simulated_world.update(500);

    // FIXME: bruh trail invalidation
    // forward_simulated_new_object->trail().push_front(m_new_object->m_pos);
    // forward_simulated_new_object->trail().update_trail(*m_simulation_view, forward_simulated_new_object->m_color);

    m_forward_simulation_is_valid = true;
}

void EssaGUI::relayout() {
    set_raw_size(sf::Vector2f(window().getSize()));
    Container::relayout();
}

void EssaGUI::update() {
    // FIXME: We should do this on every tick or just stop the main simulation.

    if(m_simulation_view->focused_object() != nullptr){
        m_focused_object_info->set_visible(true);
        m_update_focused_object_info_gui(m_simulation_view->focused_object());
    }else {
        m_focused_object_info->set_visible(false);
    }

    if (!m_forward_simulation_is_valid)
        recalculate_forward_simulation();
}

void EssaGUI::draw(sf::RenderWindow& window) const {
    if (m_new_object) {
        {
            WorldDrawScope scope(*m_simulation_view);
            m_new_object->draw(*m_simulation_view);
        }
        // FIXME: This should be drawn above grid.
        m_new_object->draw_gui(*m_simulation_view);
        m_forward_simulated_world.draw(*m_simulation_view);
    }
}

void EssaGUI::handle_event(GUI::Event& event) {
    if (event.type() == sf::Event::Closed) {
        window().close();
        event.set_handled();
    }
}
