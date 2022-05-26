#include "FPSCounter.hpp"
#include <EssaGUI/gui/Container.hpp>
#include <EssaGUI/gui/Textfield.hpp>
#include <EssaGUI/util/Units.hpp>
#include <string>

FPSCounter::FPSCounter(GUI::Container& parent) : GUI::Container(parent){
    set_layout<GUI::HorizontalBoxLayout>().set_spacing(10);

    auto label = add_widget<GUI::Textfield>();
    label->set_size({{30, Length::Percent}, Length::Auto});
    label->set_content("FPS: ");

    m_field = add_widget<GUI::Textfield>();
    m_field->set_content(std::to_string(m_fps));
}

void FPSCounter::do_update(){
    float current_time = m_clock.restart().asSeconds();
    m_fps = 1.f / (current_time);

    m_field->set_content(std::to_string(m_fps));
}
