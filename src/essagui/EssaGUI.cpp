#include "EssaGUI.hpp"

#include "EssaSettings.hpp"
#include "FocusedObjectGUI.hpp"
#include "PythonREPL.hpp"

#include "../gui/ListView.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>

class TestModel : public GUI::Model {
    virtual size_t row_count() const override {
        return 5;
    }
    virtual size_t column_count() const override {
        return 3;
    }
    virtual std::string data(size_t row, size_t column) const override {
        switch(column)
        {
            case 0: return "test";
            case 1: return std::to_string(row);
            case 2: return std::to_string(row) + " another test ";
        }
        return "";
    }
    virtual Column column(size_t column) const override {
        switch(column) {
            case 0: return {.width = 50, .name = "test"};
            case 1: return {.width = 30, .name = "TS"};
            case 2: return {.width = 200, .name=  "Unsigned Long Long Long test"};
        }
        return {};
    }
};

EssaGUI::EssaGUI(GUI::WidgetTreeRoot& wtr, World& world)
    : Container(wtr)
    , m_world(world) {
    set_layout<GUI::BasicLayout>();

    // TEST START
    {
        auto& test_window = GUI::Application::the().open_tool_window();
        test_window.center_on_screen();
        test_window.set_size({500, 500});
        auto& container = test_window.set_main_widget<GUI::Container>();
        container.set_layout<GUI::HorizontalBoxLayout>();
        auto lv = container.add_widget<GUI::ListView>();
        lv->create_and_set_model<TestModel>();
    }
    // TEST END

    m_simulation_view = add_widget<SimulationView>(world);
    m_simulation_view->set_size({ { 100, Length::Percent }, { 100, Length::Percent } });

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
                    m_simulation_view->set_focused(nullptr);

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

    m_world.m_simulation_view = m_simulation_view;

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
            // TODO: Add TextPrompt or something like this (like MessageBox)
            //       Or preferably a full FilePicker
            auto& prompt_window = GUI::Application::the().open_tool_window();
            prompt_window.set_title("Load world");
            prompt_window.set_size({ 500, 100 });
            prompt_window.center_on_screen();

            auto& container = prompt_window.set_main_widget<GUI::Container>();
            auto& container_layout = container.set_layout<GUI::VerticalBoxLayout>();
            container_layout.set_padding(20);
            container_layout.set_spacing(10);

            auto input_container = container.add_widget<GUI::Container>();
            input_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(20);
            input_container->set_size({ Length::Auto, 30.0_px });

            {
                auto label = input_container->add_widget<GUI::Textfield>();
                label->set_size({ 150.0_px, Length::Auto });
                label->set_content("File name:");
            }
            auto input = input_container->add_widget<GUI::Textbox>();
            // FIXME: Why textboxes are numeric by default
            input->set_data_type(GUI::Textbox::TEXT);
            input->set_placeholder("e.g solar.essa");

            // FIXME: Also, why buttons are red by default?
            auto button_container = container.add_widget<GUI::Container>();
            button_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(20);
            button_container->set_size({ Length::Auto, 30.0_px });
            {

                auto cancel_button = button_container->add_widget<GUI::TextButton>();
                cancel_button->set_alignment(GUI::Align::Center);
                cancel_button->set_content("Cancel");
                cancel_button->on_click = [prompt_window = &prompt_window]() {
                    prompt_window->close();
                };

                auto ok_button = button_container->add_widget<GUI::TextButton>();
                ok_button->set_alignment(GUI::Align::Center);
                ok_button->set_content("OK");
                ok_button->on_click = [this, prompt_window = &prompt_window, input]() {
                    m_settings_gui->set_world_file(input->get_content());
                    m_settings_gui->reset_simulation();
                    prompt_window->close();
                };
            }

            prompt_window.run();
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
            auto& python_repl_window = GUI::Application::the().open_tool_window();
            python_repl_window.set_title("PySSA");
            python_repl_window.set_position({ 600, 750 });
            python_repl_window.set_size({ 700, 250 });
            auto& python_repl = python_repl_window.set_main_widget<PythonREPL>();
        };
    }
}

void EssaGUI::update() {
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
