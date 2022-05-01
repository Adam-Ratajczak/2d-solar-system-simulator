#include "EssaGUI.hpp"

#include "EssaSettings.hpp"
#include "PythonREPL.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>

EssaGUI::EssaGUI(GUI::WidgetTreeRoot& wtr, World& world)
    : Container(wtr)
    , m_world(world) {
    set_layout<GUI::BasicLayout>();

    m_simulation_view = add_widget<SimulationView>(world);
    m_simulation_view->set_size({ { 100, Length::Percent }, { 100, Length::Percent } });
    m_focused_object_info = add_widget<FocusedObjectGUI>();
    m_focused_object_info->set_position({ 10.0_px_o, 10.0_px });
    m_focused_object_info->set_visible(false);

    auto home_button = add_widget<GUI::ImageButton>(load_image("../assets/homeButton.png"));
    home_button->set_position({ 10.0_px_o, 10.0_px_o });
    home_button->on_click = [this]() {
        m_simulation_view->reset();
    };
    home_button->set_tooltip_text("Reset coordinates");

    m_world.m_simulation_view = m_simulation_view;

    auto menu = add_widget<GUI::SettingsMenu>();
    menu->set_position({ 10.0_px, 10.0_px });
    {
        auto& create_menu = menu->add_entry(load_image("../assets/createButton.png"), "Create new object");
        create_menu.on_toggle = [this](bool state) {
            if(m_settings_gui->pause_simulation_on_creative_mode())
                m_pause_simulation(state);
            m_draw_forward_simulation = state;
        };
        create_menu.settings_container->set_layout<GUI::HorizontalBoxLayout>();
        create_menu.settings_container->set_size({ 550.0_px, 680.0_px });
        m_create_object_gui = create_menu.settings_container->add_widget<EssaCreateObject>(*m_simulation_view);
    }

    {
        auto& canvas_mode = menu->add_entry(load_image("../assets/canvasMode.png"), "Canvas Mode");
        canvas_mode.settings_container->set_layout<GUI::HorizontalBoxLayout>();
        canvas_mode.settings_container->set_size({ 1820.0_px, 990.0_px });
        m_canvas_mode_gui = canvas_mode.settings_container->add_widget<EssaCanvasMode>();

        canvas_mode.on_toggle = [this](bool state) {
            m_pause_simulation(state);
        };
    }

    {
        auto& settings = menu->add_entry(load_image("../assets/simulationSettings.png"), "Simulation Settings");
        settings.settings_container->set_layout<GUI::HorizontalBoxLayout>();
        settings.settings_container->set_size({ 500.0_px, 600.0_px });
        m_settings_gui = settings.settings_container->add_widget<EssaSettings>(*m_simulation_view);
    }

    {
        auto& pyssa = menu->add_entry(load_image("../assets/python.png"), "PySSA (Python scripting)", GUI::SettingsMenu::Expandable::No);
        pyssa.on_toggle = [](bool) {
            auto& python_repl_window = GUI::Application::the().open_tool_window("PySSA");
            // TODO: Support moving ToolWindows
            python_repl_window.set_position({ 600, 750 });
            python_repl_window.set_size({ 700, 250 });
            auto& python_repl = python_repl_window.set_main_widget<PythonREPL>();
        };
    }
}

void EssaGUI::update() {
    m_focused_object_info->update_from_object(m_simulation_view->focused_object());

    if (!m_create_object_gui->is_forward_simulation_valid())
        m_create_object_gui->recalculate_forward_simulation();
}

void EssaGUI::draw(sf::RenderWindow& window) const {
    if (m_create_object_gui->new_object() && m_draw_forward_simulation) {
        {
            WorldDrawScope scope(*m_simulation_view);
            m_create_object_gui->new_object()->draw(*m_simulation_view);
            m_create_object_gui->forward_simulated_new_object()->draw_closest_approaches(*m_simulation_view);
        }
        // FIXME: This should be drawn above grid.
        m_create_object_gui->forward_simulated_new_object()->draw_closest_approaches_gui(*m_simulation_view);
        m_create_object_gui->new_object()->draw_gui(*m_simulation_view);
        m_create_object_gui->forward_simulated_world().draw(*m_simulation_view);
    }
}

void EssaGUI::handle_event(GUI::Event& event) {
    if (event.type() == sf::Event::Closed) {
        window().close();
        event.set_handled();
    }
}

void EssaGUI::m_pause_simulation(bool state) {
    m_simulation_view->change_speed(!state);
    if (!state) {
        m_create_object_gui->set_new_object(nullptr);
        m_create_object_gui->forward_simulation_state(true);
        m_simulation_view->set_speed(m_saved_speed);
    }
    else {
        m_saved_speed = m_simulation_view->speed();
        m_simulation_view->set_speed(0);
    }
}
