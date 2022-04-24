#include "DateTextbox.hpp"
#include "ArrowButton.hpp"
#include "Container.hpp"
#include "Textbox.hpp"
#include "Textfield.hpp"
#include <SFML/Graphics/Color.hpp>
#include <sstream>
#include <string>

const std::string month_names[] = {
    "January", "February", "March",
    "April", "May", "June",
    "July", "August", "September",
    "October", "November", "December"
};

const unsigned seconds_in_a_day = 60 * 60 * 24;

namespace GUI {
DateTextbox::DateTextbox(Container& parent) : Container(parent){
    set_layout<VerticalBoxLayout>().set_spacing(10);
    auto main_container = add_widget<Container>();
    main_container->set_layout<HorizontalBoxLayout>().set_spacing(10);
    m_date_textfield = main_container->add_widget<Textfield>();
    std::ostringstream str;
    str << m_date;
    m_date_textfield->set_content(str.str());

    m_toggle_container_button = main_container->add_widget<StateTextButton<bool>>();
    m_toggle_container_button->set_size({50.0_px, Length::Auto});
    m_toggle_container_button->set_alignment(Align::Center);
    m_toggle_container_button->add_state("Show", 0, sf::Color::White);
    m_toggle_container_button->add_state("Hide", 1, sf::Color::White);

    m_toggle_container_button->on_change = [this](bool state){
        m_calendar_container->set_visible(state);
    };

    m_create_container();
    m_calendar_container->set_visible(false);
}

void DateTextbox::set_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color){
    m_date_textfield->set_display_attributes(bg_color, fg_color, text_color);

    m_toggle_container_button->set_bg_color(fg_color);
    m_toggle_container_button->set_fg_color(fg_color);
    m_toggle_container_button->set_text_color(bg_color);
}

void DateTextbox::m_create_container(){
    m_calendar_container = add_widget<Container>();
    m_calendar_container->set_size({Length::Auto, 120.0_px});
    m_calendar_container->set_layout<VerticalBoxLayout>();
    m_calendar_container->set_background_color(sf::Color::White);

    auto century_toggle_container = m_calendar_container->add_widget<GUI::Container>();
    century_toggle_container->set_layout<HorizontalBoxLayout>().set_spacing(10);
    auto left_century_arrow_btn = century_toggle_container->add_widget<ArrowButton>();
    left_century_arrow_btn->set_arrow_size(10);
    left_century_arrow_btn->set_arrow_type(ArrowButton::ArrowType::LEFTARROW);
    auto century_textfield = century_toggle_container->add_widget<Textfield>();
    century_textfield->set_content("1900 - 1999");
    century_textfield->set_alignment(Align::Center);
    century_textfield->set_display_attributes(sf::Color::White, sf::Color::White, sf::Color(200, 200, 200));
    auto right_century_arrow_btn = century_toggle_container->add_widget<ArrowButton>();
    right_century_arrow_btn->set_arrow_size(10);
    right_century_arrow_btn->set_arrow_type(ArrowButton::ArrowType::RIGHTARROW);

    auto year_toggle_container = m_calendar_container->add_widget<GUI::Container>();
    year_toggle_container->set_layout<HorizontalBoxLayout>().set_spacing(10);
    auto left_year_arrow_btn = year_toggle_container->add_widget<ArrowButton>();
    left_year_arrow_btn->set_arrow_size(10);
    left_year_arrow_btn->set_arrow_type(ArrowButton::ArrowType::LEFTARROW);
    auto year_textfield = year_toggle_container->add_widget<Textfield>();
    year_textfield->set_content(std::to_string(m_century - 1) + std::to_string(m_year));
    year_textfield->set_alignment(Align::Center);
    year_textfield->set_display_attributes(sf::Color::White, sf::Color::White, sf::Color(200, 200, 200));
    auto right_year_arrow_btn = year_toggle_container->add_widget<ArrowButton>();
    right_year_arrow_btn->set_arrow_size(10);
    right_year_arrow_btn->set_arrow_type(ArrowButton::ArrowType::RIGHTARROW);

    auto month_toggle_container = m_calendar_container->add_widget<GUI::Container>();
    month_toggle_container->set_layout<HorizontalBoxLayout>().set_spacing(10);
    auto left_month_arrow_btn = month_toggle_container->add_widget<ArrowButton>();
    left_month_arrow_btn->set_arrow_size(10);
    left_month_arrow_btn->set_arrow_type(ArrowButton::ArrowType::LEFTARROW);
    auto month_textfield = month_toggle_container->add_widget<Textfield>();
    month_textfield->set_content(month_names[m_month - 1]);
    month_textfield->set_alignment(Align::Center);
    month_textfield->set_display_attributes(sf::Color::White, sf::Color::White, sf::Color(200, 200, 200));
    auto right_month_arrow_btn = month_toggle_container->add_widget<ArrowButton>();
    right_month_arrow_btn->set_arrow_size(10);
    right_month_arrow_btn->set_arrow_type(ArrowButton::ArrowType::RIGHTARROW);
    
}

}