#include "EssaSettings.hpp"
#include "World.hpp"
#include "glwrapper/Sphere.hpp"
#include "gui/Container.hpp"
#include "gui/StateTextButton.hpp"
#include "gui/TabWidget.hpp"
#include "gui/Textbox.hpp"
#include "gui/Textfield.hpp"
#include "gui/ValueSlider.hpp"
#include "util/SimulationClock.hpp"

EssaSettings::EssaSettings(GUI::Container& c, SimulationView& simulation_view)
    : GUI::Container(c)
    , m_simulation_view(simulation_view) {
    set_layout<GUI::VerticalBoxLayout>().set_spacing(10);

    auto title_label = add_widget<GUI::Textfield>();
    title_label->set_content("Settings");
    title_label->set_size({ Length::Auto, 30.0_px });
    title_label->set_alignment(GUI::Align::Center);

    auto tab_widget = add_widget<GUI::TabWidget>();
    auto& simulation_settings = tab_widget->add_tab("Simulation");
    {
        auto& layout = simulation_settings.set_layout<GUI::VerticalBoxLayout>();
        layout.set_spacing(10);
        layout.set_padding(10);
        auto iterations_control = simulation_settings.add_widget<GUI::ValueSlider>(1, 1000);
        iterations_control->set_name("Iterations");
        iterations_control->set_unit("i/t");
        m_on_restore_defaults.push_back([iterations_control]() {
            iterations_control->set_value(10);
        });
        iterations_control->set_tooltip_text("Count of simulation ticks per render frame");
        iterations_control->on_change = [this](double value) {
            if (value > 0)
                m_simulation_view.set_iterations(value);
        };
        iterations_control->set_class_name("Simulation");

        auto tick_length_control = simulation_settings.add_widget<GUI::ValueSlider>(60, 60 * 60 * 24, 60);
        tick_length_control->set_name("Tick Length");
        tick_length_control->set_unit("s/t");
        m_on_restore_defaults.push_back([tick_length_control]() {
            tick_length_control->set_value(60 * 60 * 12); // 12h / half a day
        });
        tick_length_control->set_tooltip_text("Amount of simulation seconds per simulation tick (Affects accuracy)");
        tick_length_control->on_change = [this](double value) {
            if (value > 0)
                m_simulation_view.world().set_simulation_seconds_per_tick(value);
        };

        auto reset_trails_button = simulation_settings.add_widget<GUI::TextButton>();
        reset_trails_button->set_size({ Length::Auto, 30.0_px });
        reset_trails_button->set_content("Clear Trails");
        reset_trails_button->on_click = [&]() {
            m_simulation_view.world().reset_all_trails();
        };
    }

    auto& display_settings = tab_widget->add_tab("Display");
    display_settings.set_layout<GUI::VerticalBoxLayout>().set_spacing(10);
    {
        auto& layout = display_settings.set_layout<GUI::VerticalBoxLayout>();
        layout.set_spacing(10);
        layout.set_padding(10);
        auto fov_control = display_settings.add_widget<GUI::ValueSlider>(20, 160, 1);
        fov_control->set_name("FOV");
        fov_control->set_unit("deg");
        m_on_restore_defaults.push_back([fov_control]() {
            fov_control->set_value(80);
        });
        fov_control->set_tooltip_text("Field of View");
        fov_control->on_change = [this](double value) {
            if (value > 0)
                m_simulation_view.set_fov(Angle { static_cast<float>(value), Angle::Unit::Deg });
        };

        auto toggle_sphere_mode_container = display_settings.add_widget<GUI::Container>();
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
        m_on_restore_defaults.push_back([toggle_sphere_mode]() {
            toggle_sphere_mode->set_index(0);
        });
        toggle_sphere_mode->on_change = [this](Sphere::DrawMode mode) {
            this->m_simulation_view.world().for_each_object([mode](Object& object) {
                object.sphere().set_draw_mode(mode);
            });
        };

        auto toggle_time_format_container = display_settings.add_widget<GUI::Container>();
        auto& toggle_time_format_layout = toggle_time_format_container->set_layout<GUI::HorizontalBoxLayout>();
        toggle_time_format_layout.set_spacing(10);
        toggle_time_format_container->set_size({ Length::Auto, 30.0_px });

        auto toggle_time_format_label = toggle_time_format_container->add_widget<GUI::Textfield>();
        toggle_time_format_label->set_content("Toggle time format: ");

        auto toggle_time_format = toggle_time_format_container->add_widget<GUI::StateTextButton<Util::SimulationClock::Format>>();
        toggle_time_format->set_alignment(GUI::Align::Center);
        toggle_time_format->add_state("American format", Util::SimulationClock::Format::AMERICAN, sf::Color::Blue);
        toggle_time_format->add_state("Short format", Util::SimulationClock::Format::SHORT_TIME, sf::Color::Blue);
        toggle_time_format->add_state("Mid format", Util::SimulationClock::Format::MID_TIME, sf::Color::Blue);
        toggle_time_format->add_state("Long format", Util::SimulationClock::Format::LONG_TIME, sf::Color::Blue);
        toggle_time_format->add_state("No clock American format", Util::SimulationClock::Format::NO_CLOCK_AMERICAN, sf::Color::Blue);
        toggle_time_format->add_state("No clock short format", Util::SimulationClock::Format::NO_CLOCK_SHORT, sf::Color::Blue);
        toggle_time_format->add_state("No clock mid format", Util::SimulationClock::Format::NO_CLOCK_MID, sf::Color::Blue);
        toggle_time_format->add_state("No clock long format", Util::SimulationClock::Format::NO_CLOCK_LONG, sf::Color::Blue);

        m_on_restore_defaults.push_back([toggle_time_format]() {
            toggle_time_format->set_index(0);
        });

        toggle_time_format->on_change = [](Util::SimulationClock::Format state) {
            Util::SimulationClock::time_format = state;
        };

        auto add_toggle = [&](std::string title, auto on_change, bool default_value = true) {
            auto toggle_container = display_settings.add_widget<GUI::Container>();
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
            m_on_restore_defaults.push_back([toggle, default_value]() {
                toggle->set_active(default_value);
            });
            return toggle;
        };

        auto show_labels_toggle = add_toggle("Show labels", [this](bool state) {
            this->m_simulation_view.toggle_label_visibility(state);
        });
        auto show_grid_toggle = add_toggle("Show grid", [this](bool state) {
            this->m_simulation_view.set_show_grid(state);
        });
        auto show_trails_toggle = add_toggle("Show trails", [this](bool state) {
            this->m_simulation_view.set_show_trails(state);
        });
    }

    auto restore_sim_container = add_widget<GUI::Container>();
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
    restore_sim->set_tooltip_text("Restore Simulation to state from the beginning");
    restore_sim->on_click = [world = &m_simulation_view.world()]() {
        world->reset();
    };

    auto restore_defaults_container = add_widget<GUI::Container>();
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

    restore_defaults->on_click = [this]() {
        for (auto& func : m_on_restore_defaults)
            func();
    };
    restore_defaults->on_click();
}