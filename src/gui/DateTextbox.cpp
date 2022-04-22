#include "DateTextbox.hpp"
#include "Textbox.hpp"
#include "Textfield.hpp"
#include <sstream>

namespace GUI {
DateTextbox::DateTextbox(Container& parent) : Container(parent){
    set_layout<HorizontalBoxLayout>().set_spacing(10);
    m_date_textfield = add_widget<Textfield>();
    std::ostringstream str;
    str << m_date;
    m_date_textfield->set_content(str.str());

    m_toggle_container_button = add_widget<StateTextButton<bool>>();
    m_toggle_container_button->set_size({50.0_px, Length::Auto});
    m_toggle_container_button->set_alignment(Align::Center);
    m_toggle_container_button->add_state("Show", 0, sf::Color::White);
    m_toggle_container_button->add_state("Hide", 1, sf::Color::White);

    m_toggle_container_button->on_change = [](bool state){

    };
}

void DateTextbox::set_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color){
    m_date_textfield->set_display_attributes(bg_color, fg_color, text_color);

    m_toggle_container_button->set_bg_color(fg_color);
    m_toggle_container_button->set_fg_color(fg_color);
    m_toggle_container_button->set_text_color(bg_color);
}

}