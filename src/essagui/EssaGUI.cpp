#include "EssaGUI.hpp"

#include "EssaSettings.hpp"
#include "FocusedObjectGUI.hpp"
#include "PythonREPL.hpp"

#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/FilePrompt.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Mutex.hpp>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

EssaGUI::EssaGUI(GUI::WidgetTreeRoot& wtr, World& world)
    : Container(wtr)
    , m_world(world) {
    set_layout<GUI::BasicLayout>();

    m_simulation_view = add_widget<SimulationView>(world);
    m_simulation_view->set_size({ { 100, Length::Percent }, { 100, Length::Percent } });
    // m_simulation_view->set_visible(false);
    m_world.m_simulation_view = m_simulation_view;

    m_simulation_view->on_change_focus = [&](Object* obj) {
        if (obj == nullptr)
            return;

        auto focused_object_window = GUI::Application::the().open_or_focus_tool_window(obj->name(), "FocusedGUI-" + obj->name());
        if (focused_object_window.opened) {
            focused_object_window.window->set_position({ size().x - 550, 50 });
            focused_object_window.window->set_size({ 500, 600 });
            auto focused_object_gui = focused_object_window.window->set_main_widget<FocusedObjectGUI>(obj, focused_object_window.window, world);
            focused_object_gui.update_params();
            focused_object_window.window->on_close = [&]() {
                if (m_settings_gui->unfocus_on_wnd_close())
                    m_simulation_view->set_focused_object(nullptr);

                if (m_simulation_view->speed() == 0)
                    m_simulation_view->pause_simulation(false);
            };
        }
    };

    auto home_button = add_widget<GUI::ImageButton>(load_image("../assets/homeButton.png"));
    home_button->set_position({ 10.0_px_o, 10.0_px_o });
    home_button->on_click = [this]() {
        m_simulation_view->reset();
    };
    home_button->set_tooltip_text("Reset coordinates");

    auto menu = add_widget<GUI::SettingsMenu>();
    menu->set_position({ 10.0_px, 10.0_px });
    {
        auto& create_menu = menu->add_entry(load_image("../assets/createButton.png"), "Create new object");
        create_menu.on_toggle = [this](bool state) {
            if (m_settings_gui->pause_simulation_on_creative_mode()) {
                m_create_object_gui->set_new_object(nullptr);
                m_create_object_gui->forward_simulation_state(true);
                m_simulation_view->pause_simulation(state);
            }
            m_draw_forward_simulation = state;
        };
        create_menu.settings_container->set_layout<GUI::HorizontalBoxLayout>();
        create_menu.settings_container->set_size({ 550.0_px, 680.0_px });
        m_create_object_gui = create_menu.settings_container->add_widget<EssaCreateObject>(*m_simulation_view);
    }

    {
        auto& load_world = menu->add_entry(load_image("../assets/loadWorld.png"), "Load world", GUI::SettingsMenu::Expandable::No);
        load_world.on_toggle = [this](bool) {
            auto& prompt = GUI::Application::the().open_overlay<GUI::FilePrompt>("Choose file to open: ", "Open file", "e.g.: solar.essa");
            prompt.add_desired_extension(".essa");
            prompt.run();

            if (prompt.result().has_value())
                m_settings_gui->load_world(prompt.result().value().toAnsiString());
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
        pyssa.on_toggle = [this](bool) {
            open_python_repl();
        };
    }
}

void EssaGUI::open_python_repl() {
    auto& python_repl_window = GUI::Application::the().open_overlay<GUI::ToolWindow>();
    python_repl_window.set_title("PySSA");
    python_repl_window.set_position({ 600, 750 });
    python_repl_window.set_size({ 700, 250 });
    GUI::Application::the().focus_overlay(python_repl_window);
    python_repl_window.set_main_widget<PythonREPL>();
}

void EssaGUI::update() {
    if (!m_create_object_gui->is_forward_simulation_valid())
        m_create_object_gui->recalculate_forward_simulation();
}

void EssaGUI::draw(GUI::SFMLWindow& window) const {
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
    else if (event.type() == sf::Event::KeyPressed && event.event().key.code == sf::Keyboard::Tilde) {
        open_python_repl();
    }
}
