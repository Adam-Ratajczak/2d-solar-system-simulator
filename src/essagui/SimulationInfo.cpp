#include "SimulationInfo.hpp"
#include <EssaGUI/gui/Container.hpp>
#include <EssaGUI/gui/Textfield.hpp>
#include <EssaGUI/util/Units.hpp>
#include <string>

SimulationInfo::SimulationInfo(GUI::Container& parent, SimulationView* sw) : GUI::Container(parent), m_simulation_view(sw){
    set_layout<GUI::VerticalBoxLayout>().set_spacing(10);

    auto fps_container = add_widget<GUI::Container>();
    fps_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
    auto fps_label = fps_container->add_widget<GUI::Textfield>();
    fps_label->set_size({{30, Length::Percent}, Length::Auto});
    fps_label->set_content("FPS: ");

    m_fps_field = fps_container->add_widget<GUI::Textfield>();
    m_fps_field->set_content("N / A");

    auto speed_container = add_widget<GUI::Container>();
    speed_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);
    auto speed_label = speed_container->add_widget<GUI::Textfield>();
    speed_label->set_size({{30, Length::Percent}, Length::Auto});
    speed_label->set_content("Speed: ");

    m_speed_field = speed_container->add_widget<GUI::Textfield>();
    m_speed_field->set_content("x " + std::to_string((int)std::fabs(m_simulation_view->speed())) + ((m_simulation_view->speed() < 0) ? " (Reversed)" : ""));
}

void SimulationInfo::do_update(){
    float current_time = m_clock.restart().asSeconds();
    m_fps = 1.f / (current_time);

    m_fps_field->set_content(std::to_string(m_fps));
    m_speed_field->set_content("x " + std::to_string((int)std::fabs(m_simulation_view->speed())) + ((m_simulation_view->speed() < 0) ? " (Reversed)" : ""));
}
