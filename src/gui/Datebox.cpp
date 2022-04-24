#include "Datebox.hpp"
#include "ArrowButton.hpp"
#include "Container.hpp"
#include "Textbox.hpp"
#include "Textfield.hpp"
#include <SFML/Graphics/Color.hpp>
#include <iomanip>
#include <sstream>
#include <string>

const std::string month_names[] = {
    "January", "February", "March",
    "April", "May", "June",
    "July", "August", "September",
    "October", "November", "December"
};

namespace GUI {

Datebox::Datebox(Container& parent)
    : Container(parent) {
    set_layout<VerticalBoxLayout>().set_spacing(10);
    auto main_container = add_widget<Container>();
    main_container->set_layout<HorizontalBoxLayout>().set_spacing(10);
    main_container->set_size({ Length::Auto, 30.0_px });
    m_date_textfield = main_container->add_widget<Textfield>();
    std::ostringstream str;
    str << m_date;
    m_date_textfield->set_content(str.str());

    m_toggle_container_button = main_container->add_widget<StateTextButton<bool>>();
    m_toggle_container_button->set_size({ 50.0_px, Length::Auto });
    m_toggle_container_button->set_alignment(Align::Center);
    m_toggle_container_button->add_state("Show", 0, sf::Color::White);
    m_toggle_container_button->add_state("Hide", 1, sf::Color::White);
    // m_date = Util::SimulationTime::create(1602, 7, 21);
    m_create_container();

    m_toggle_container_button->on_change = [this](bool state) {
        m_calendar_container->set_visible(state);
        m_update_calendar();
        if (state)
            set_size({ Length::Auto, 280.0_px });
        else
            set_size({ Length::Auto, 30.0_px });
    };

    m_calendar_container->set_visible(false);
}

void Datebox::set_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color) {
    m_date_textfield->set_display_attributes(bg_color, fg_color, text_color);

    m_toggle_container_button->set_bg_color(fg_color);
    m_toggle_container_button->set_fg_color(fg_color);
    m_toggle_container_button->set_text_color(bg_color);
}

TextButton* Datebox::m_create_calendar_button(Container& c) const {
    auto btn = c.add_widget<TextButton>();
    btn->set_content("");
    btn->set_active_content("");
    btn->set_alignment(Align::Center);
    btn->set_display_attributes(sf::Color(220, 220, 220), sf::Color(220, 220, 220), sf::Color(100, 100, 100));
    btn->set_active_display_attributes(sf::Color(180, 180, 180), sf::Color(180, 180, 180), sf::Color(50, 50, 50));

    btn->on_change = [btn, this](bool state) {
        for (auto& field : m_calendar_contents) {
            if (field != btn)
                field->set_active_without_action(false);
        }
    };

    return btn;
}

void Datebox::m_create_container() {
    time_t clock = m_date.time_since_epoch().count();
    tm local_tm = *localtime(&clock);

    m_calendar_container = add_widget<Container>();
    m_calendar_container->set_layout<VerticalBoxLayout>().set_spacing(2);
    m_calendar_container->set_background_color(sf::Color::White);

    auto century_toggle_container = m_calendar_container->add_widget<GUI::Container>();
    century_toggle_container->set_layout<HorizontalBoxLayout>().set_spacing(10);
    century_toggle_container->set_size({ Length::Auto, 20.0_px });
    auto left_century_arrow_btn = century_toggle_container->add_widget<ArrowButton>();
    left_century_arrow_btn->set_arrow_color(sf::Color(200, 200, 200));
    left_century_arrow_btn->set_arrow_size(10);
    left_century_arrow_btn->set_arrow_type(ArrowButton::ArrowType::LEFTARROW);
    m_century_textfield = century_toggle_container->add_widget<Textfield>();
    m_century_textfield->set_alignment(Align::Center);
    m_century_textfield->set_display_attributes(sf::Color::White, sf::Color::White, sf::Color(200, 200, 200));
    auto right_century_arrow_btn = century_toggle_container->add_widget<ArrowButton>();
    right_century_arrow_btn->set_arrow_color(sf::Color(200, 200, 200));
    right_century_arrow_btn->set_arrow_size(10);
    right_century_arrow_btn->set_arrow_type(ArrowButton::ArrowType::RIGHTARROW);

    auto year_toggle_container = m_calendar_container->add_widget<GUI::Container>();
    year_toggle_container->set_layout<HorizontalBoxLayout>().set_spacing(10);
    year_toggle_container->set_size({ Length::Auto, 20.0_px });
    auto left_year_arrow_btn = year_toggle_container->add_widget<ArrowButton>();
    left_year_arrow_btn->set_arrow_color(sf::Color(200, 200, 200));
    left_year_arrow_btn->set_arrow_size(10);
    left_year_arrow_btn->set_arrow_type(ArrowButton::ArrowType::LEFTARROW);
    m_year_textfield = year_toggle_container->add_widget<Textfield>();
    m_year_textfield->set_alignment(Align::Center);
    m_year_textfield->set_display_attributes(sf::Color::White, sf::Color::White, sf::Color(200, 200, 200));
    auto right_year_arrow_btn = year_toggle_container->add_widget<ArrowButton>();
    right_year_arrow_btn->set_arrow_color(sf::Color(200, 200, 200));
    right_year_arrow_btn->set_arrow_size(10);
    right_year_arrow_btn->set_arrow_type(ArrowButton::ArrowType::RIGHTARROW);

    auto month_toggle_container = m_calendar_container->add_widget<GUI::Container>();
    month_toggle_container->set_layout<HorizontalBoxLayout>().set_spacing(10);
    month_toggle_container->set_size({ Length::Auto, 20.0_px });
    auto left_month_arrow_btn = month_toggle_container->add_widget<ArrowButton>();
    left_month_arrow_btn->set_arrow_color(sf::Color(200, 200, 200));
    left_month_arrow_btn->set_arrow_size(10);
    left_month_arrow_btn->set_arrow_type(ArrowButton::ArrowType::LEFTARROW);
    m_month_textfield = month_toggle_container->add_widget<Textfield>();
    m_month_textfield->set_alignment(Align::Center);
    m_month_textfield->set_display_attributes(sf::Color::White, sf::Color::White, sf::Color(200, 200, 200));
    auto right_month_arrow_btn = month_toggle_container->add_widget<ArrowButton>();
    right_month_arrow_btn->set_arrow_color(sf::Color(200, 200, 200));
    right_month_arrow_btn->set_arrow_size(10);
    right_month_arrow_btn->set_arrow_type(ArrowButton::ArrowType::RIGHTARROW);

    auto day_selection_container = m_calendar_container->add_widget<Container>();
    day_selection_container->set_layout<VerticalBoxLayout>().set_spacing(2);
    for (unsigned i = 0; i < 6; i++) {
        m_final_row = day_selection_container->add_widget<Container>();
        m_final_row->set_layout<HorizontalBoxLayout>().set_spacing(2);
        m_final_row->set_size({ Length::Auto, { 100.0 / 6, Length::Percent } });
        for (unsigned i = 0; i < 7; i++)
            m_calendar_contents.push_back(m_create_calendar_button(*m_final_row));
    }

    left_century_arrow_btn->on_click = [this]() mutable {
        time_t clock = m_date.time_since_epoch().count();
        tm local_tm = *localtime(&clock);

        m_date = Util::SimulationTime::create(1900 + local_tm.tm_year - 100, local_tm.tm_mon + 1, local_tm.tm_mday + 1);
        m_update_calendar();
    };

    left_year_arrow_btn->on_click = [this]() mutable {
        time_t clock = m_date.time_since_epoch().count();
        tm local_tm = *localtime(&clock);

        m_date = Util::SimulationTime::create(1900 + local_tm.tm_year - 1, local_tm.tm_mon + 1, local_tm.tm_mday + 1);
        m_update_calendar();
    };

    left_month_arrow_btn->on_click = [this]() mutable {
        time_t clock = m_date.time_since_epoch().count();
        tm local_tm = *localtime(&clock);

        m_date = Util::SimulationTime::create(1900 + local_tm.tm_year, local_tm.tm_mon, local_tm.tm_mday + 1);
        m_update_calendar();
    };

    right_century_arrow_btn->on_click = [this]() mutable {
        time_t clock = m_date.time_since_epoch().count();
        tm local_tm = *localtime(&clock);

        m_date = Util::SimulationTime::create(1900 + local_tm.tm_year + 100, local_tm.tm_mon + 1, local_tm.tm_mday);
        m_update_calendar();
    };

    right_year_arrow_btn->on_click = [this]() mutable {
        time_t clock = m_date.time_since_epoch().count();
        tm local_tm = *localtime(&clock);

        m_date = Util::SimulationTime::create(1900 + local_tm.tm_year + 1, local_tm.tm_mon + 1, local_tm.tm_mday);
        m_update_calendar();
    };

    right_month_arrow_btn->on_click = [this]() mutable {
        time_t clock = m_date.time_since_epoch().count();
        tm local_tm = *localtime(&clock);

        m_date = Util::SimulationTime::create(1900 + local_tm.tm_year, local_tm.tm_mon + 2, local_tm.tm_mday);
        m_update_calendar();
    };
}

void Datebox::m_update_calendar() {
    time_t clock = m_date.time_since_epoch().count();
    tm local_tm = *localtime(&clock);

    if (local_tm.tm_year < 0)
        m_century_textfield->set_content(std::to_string(1900 + (int)((local_tm.tm_year / 100 - 1) * 100)) + " - " + std::to_string(1900 + (int)((local_tm.tm_year / 100 - 1) * 100) + 99));
    else
        m_century_textfield->set_content(std::to_string(1900 + (int)(local_tm.tm_year / 100 * 100)) + " - " + std::to_string(1900 + (int)(local_tm.tm_year / 100 * 100) + 99));
    m_year_textfield->set_content(std::to_string(1900 + local_tm.tm_year));
    m_month_textfield->set_content(month_names[local_tm.tm_mon]);

    int i = -local_tm.tm_wday - 1;
    std::ostringstream str;
    str << m_date;
    m_date_textfield->set_content(str.str());

    for (unsigned j = 0; j < m_calendar_contents.size(); j++) {
        i++;
        Util::SimulationClock::time_point date = Util::SimulationTime::create(1900 + local_tm.tm_year, local_tm.tm_mon + 1, i);
        auto t = date.time_since_epoch().count();
        tm temp_tm = *localtime(&t);

        if (j == 35 && temp_tm.tm_mday <= 7) {
            m_final_row->set_visible(false);
            return;
        }
        else {
            m_final_row->set_visible(true);
        }

        m_calendar_contents[j]->set_content(std::to_string(temp_tm.tm_mday));
        m_calendar_contents[j]->set_active_content(std::to_string(temp_tm.tm_mday));
        m_calendar_contents[j]->set_display_attributes(
            temp_tm.tm_mon == local_tm.tm_mon ? sf::Color(150, 150, 150) : sf::Color(200, 200, 200), {},
            temp_tm.tm_mon == local_tm.tm_mon ? sf::Color::Black : sf::Color(50, 50, 50));
    }
}

}