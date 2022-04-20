#include "EssaGUI.hpp"

#include "Object.hpp"
#include "EssaSettings.hpp"
#include "SimulationView.hpp"
#include "World.hpp"
#include "glwrapper/Sphere.hpp"
#include "gui/Button.hpp"
#include "gui/Container.hpp"
#include "gui/ImageButton.hpp"
#include "gui/PythonREPL.hpp"
#include "gui/StateTextButton.hpp"
#include "gui/TabWidget.hpp"
#include "gui/TextButton.hpp"
#include "gui/Textbox.hpp"
#include "gui/Textfield.hpp"
#include "util/SimulationClock.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>

sf::Image load_image(std::string path) {
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
        m_add_object_button->set_visible(true);
        m_forward_simulation_is_valid = false;
        m_new_object_pos = pos;
    };

    m_simulation_view->on_focus_measure = [&](Object* focusing) {
        m_add_object_button->set_visible(true);
        m_focused = focusing;
    };
    m_focused_object_info = add_widget<FocusedObjectGUI>();
    m_focused_object_info->set_position({ 10.0_px_o, 10.0_px });
    m_focused_object_info->set_visible(false);

    m_home_button = add_widget<GUI::ImageButton>(load_image("../assets/homeButton.png"));
    m_home_button->set_position({ 10.0_px_o, 10.0_px_o });
    m_home_button->on_click = [this]() {
        m_simulation_view->reset();
    };
    m_home_button->set_tooltip_text("Reset coordinates");

    auto python_repl = add_widget<GUI::PythonREPL>();
    python_repl->set_position({ 600.0_px, 10.0_px_o });
    python_repl->set_size({ 700.0_px, 250.0_px });

    m_world.m_simulation_view = m_simulation_view.get();

    auto menu = add_widget<GUI::SettingsMenu>();
    menu->set_position({ 10.0_px, 10.0_px });
    {
        auto& create_menu = menu->add_entry(load_image("../assets/createButton.png"), "Create new object");
        create_menu.on_toggle = [this](bool state) {
            m_simulation_view->change_speed(!state);
            if (!state) {
                m_new_object = nullptr;
                m_forward_simulation_is_valid = true;
                m_simulation_view->set_speed(m_saved_speed);
            }
            else {
                m_saved_speed = m_simulation_view->speed();
                m_simulation_view->set_speed(0);
            }
        };

        // TODO: Shrink-to-fit
        create_menu.settings_container->set_size({ 500.0_px, 680.0_px });
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

                auto creative_mode_button = m_submit_container->add_widget<GUI::ImageButton>(load_image("../assets/toggleCreativeModeButton.png"));
                creative_mode_button->set_toggleable(true);
                creative_mode_button->set_tooltip_text("Toggle automatic orbit calculation");

                creative_mode_button->on_change = [this](bool state) {
                    m_create_object_from_params_container->set_visible(!state);
                    m_create_object_from_orbit_container->set_visible(state);
                    m_toggle_orbit_direction_button->set_visible(state);
                    m_automatic_orbit_calculation = state;
                };

                m_toggle_unit_button = m_create_toggle_unit_button();

                m_toggle_orbit_direction_button = m_submit_container->add_widget<GUI::ImageButton>(load_image("../assets/orbitDirectionButton.png"));
                m_toggle_orbit_direction_button->set_toggleable(true);
                m_toggle_orbit_direction_button->set_tooltip_text("Toggle orbitting body direction");
                m_toggle_orbit_direction_button->on_change = [](bool state) {
                };
                m_toggle_orbit_direction_button->set_visible(false);

                m_submit_container->add_widget<Widget>(); // spacer

                m_add_object_button = m_submit_container->add_widget<GUI::ImageButton>(load_image("../assets/addObjectButton.png"));
                m_add_object_button->on_click = [&world, this]() {
                    if (m_automatic_orbit_calculation)
                        world.add_object(m_create_object_from_orbit());
                    else
                        world.add_object(m_create_object_from_params());
                    m_simulation_view->m_measured = false;
                };
            }
        }
    }

    {
        auto& canvas_mode = menu->add_entry(load_image("../assets/canvasMode.png"), "Canvas Mode");
        m_create_cavas_mode_gui(*canvas_mode.settings_container);
    }

    {
        auto& settings = menu->add_entry(load_image("../assets/simulationSettings.png"), "Simulation Settings");
        settings.settings_container->set_layout<GUI::HorizontalBoxLayout>();
        settings.settings_container->set_size({500.0_px, 500.0_px});
        settings.settings_container->add_widget<EssaSettings>(*m_simulation_view);
    }
}

void EssaGUI::m_recalculate_forward_simulation() {
    if (m_automatic_orbit_calculation)
        m_new_object = m_create_object_from_orbit();
    else
        m_new_object = m_create_object_from_params();

    if (!m_new_object)
        return;

    m_world.clone_for_forward_simulation(m_forward_simulated_world);

    // We need trail of the forward simulated object but
    // the object itself will be drawn at current position.
    m_forward_simulated_world.add_object(m_new_object->clone_for_forward_simulation(m_forward_simulated_world));

    m_forward_simulated_world.update(m_forward_simulation_ticks_control->value());

    m_forward_simulation_is_valid = true;
}

void EssaGUI::relayout() {
    set_raw_size(sf::Vector2f(window().getSize()));
    Container::relayout();
}

void EssaGUI::update() {
    m_focused_object_info->update_from_object(m_simulation_view->focused_object());

    if (!m_forward_simulation_is_valid)
        m_recalculate_forward_simulation();
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
