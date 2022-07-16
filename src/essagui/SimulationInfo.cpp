#include "SimulationInfo.hpp"
#include "../World.hpp"
#include <EssaGUI/gui/Container.hpp>
#include <EssaGUI/gui/Textfield.hpp>
#include <EssaUtil/Units.hpp>
#include <string>

SimulationInfo::SimulationInfo(GUI::Container& parent, SimulationView* sw)
    : GUI::Container(parent)
    , m_simulation_view(sw) {
    set_layout<GUI::VerticalBoxLayout>().set_spacing(10);

    auto fps_container = add_widget<GUI::Container>();
    fps_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
    auto fps_label = fps_container->add_widget<GUI::Textfield>();
    fps_label->set_size({ { 30, Length::Percent }, Length::Auto });
    fps_label->set_content("FPS: ");

    m_fps_field = fps_container->add_widget<GUI::Textfield>();
    m_fps_field->set_content("60.000000");

    auto speed_container = add_widget<GUI::Container>();
    speed_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
    auto time_label = speed_container->add_widget<GUI::Textfield>();
    time_label->set_size({ { 30, Length::Percent }, Length::Auto });
    time_label->set_content("Time: ");

    m_time_field = speed_container->add_widget<GUI::Textfield>();
    m_update_time();
}

void SimulationInfo::do_update() {
    m_update_fps();
    m_update_time();
}

void SimulationInfo::m_update_fps() {
    // TODO
    // float current_time = m_clock.restart().asSeconds();
    m_fps = 60; // 1.f / (current_time);

    m_fps_field->set_content(Util::UString { std::to_string(m_fps) });
}

void SimulationInfo::m_update_time() {
    std::ostringstream oss;
    oss << m_simulation_view->world().date();
    if (m_simulation_view->speed() == 0)
        oss << " (Paused";
    else
        oss << " (" << std::abs(m_simulation_view->speed()) << "x";

    if (m_simulation_view->speed() < 0)
        oss << ", Reversed";
    oss << ")";

    m_time_field->set_content(Util::UString { oss.str() });
}
