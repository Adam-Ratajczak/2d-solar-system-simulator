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

EssaGUI::EssaGUI(GUI::Application& application, World& world)
    : Container(application)
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
                m_create_object_gui->set_new_object(nullptr);
                m_create_object_gui->forward_simulation_state(true);
                m_simulation_view->set_speed(m_saved_speed);
            }
            else {
                m_saved_speed = m_simulation_view->speed();
                m_simulation_view->set_speed(0);
            }
        };
        create_menu.settings_container->set_layout<GUI::HorizontalBoxLayout>();
        create_menu.settings_container->set_size({550.0_px, 680.0_px});
        m_create_object_gui = create_menu.settings_container->add_widget<EssaCreateObject>(*m_simulation_view);
    }

    {
        auto& canvas_mode = menu->add_entry(load_image("../assets/canvasMode.png"), "Canvas Mode");
        m_create_cavas_mode_gui(*canvas_mode.settings_container);
    }

    {
        auto& settings = menu->add_entry(load_image("../assets/simulationSettings.png"), "Simulation Settings");
        settings.settings_container->set_layout<GUI::HorizontalBoxLayout>();
        settings.settings_container->set_size({500.0_px, 500.0_px});
        m_settings_gui = settings.settings_container->add_widget<EssaSettings>(*m_simulation_view);
    }
}

void EssaGUI::relayout() {
    set_raw_size(sf::Vector2f(window().getSize()));
    // std::cout << m_create_object_gui.get() << "\n";
    Container::relayout();
}

void EssaGUI::update() {
    m_focused_object_info->update_from_object(m_simulation_view->focused_object());

    if (!m_create_object_gui->is_forward_simulation_valid())
        m_create_object_gui->recalculate_forward_simulation();
}

void EssaGUI::draw(sf::RenderWindow& window) const {
    if (m_create_object_gui->new_object()) {
        {
            WorldDrawScope scope(*m_simulation_view);
            m_create_object_gui->forward_simulated_new_object()->draw_closest_approaches(*m_simulation_view);
            m_create_object_gui->new_object()->draw(*m_simulation_view);
        }
        // FIXME: This should be drawn above grid.
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
