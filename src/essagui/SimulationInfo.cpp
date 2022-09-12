#include "SimulationInfo.hpp"
#include "../World.hpp"
#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/Container.hpp>
#include <EssaGUI/gui/Textfield.hpp>
#include <EssaUtil/Units.hpp>
#include <string>

SimulationInfo::SimulationInfo(SimulationView* sw)
    : m_simulation_view(sw) {
}

void SimulationInfo::on_init() {
    set_layout<GUI::VerticalBoxLayout>().set_spacing(10);

    auto fps_container = add_widget<GUI::Container>();
    fps_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
    auto fps_label = fps_container->add_widget<GUI::Textfield>();
    fps_label->set_size({ 50.0_px, Length::Auto });
    fps_label->set_content("FPS: ");

    m_fps_field = fps_container->add_widget<GUI::Textfield>();
    m_fps_field->set_content("60.000000");

    auto speed_container = add_widget<GUI::Container>();
    speed_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
    auto time_label = speed_container->add_widget<GUI::Textfield>();
    time_label->set_size({ 50.0_px, Length::Auto });
    time_label->set_content("Time: ");

    m_time_field = speed_container->add_widget<GUI::Textfield>();
    m_time_field->set_size({ Length::Auto, Length::Auto });
    m_update_time();
}

void SimulationInfo::do_update() {
    m_update_fps();
    m_update_time();
}

void SimulationInfo::m_update_fps() {
    m_fps = GUI::Application::the().tps();
    m_fps_field->set_content(Util::UString { std::to_string(m_fps) });
}

void SimulationInfo::m_update_time() {
    std::ostringstream oss;
    oss << m_simulation_view->world().date() << " (";
    oss << std::abs(m_simulation_view->raw_speed()) << "x";

    if (m_simulation_view->is_paused())
        oss << ", Paused";
    if (m_simulation_view->raw_speed() < 0)
        oss << ", Reversed";

    oss << ")";
    m_time_field->set_content(Util::UString { oss.str() });
}
