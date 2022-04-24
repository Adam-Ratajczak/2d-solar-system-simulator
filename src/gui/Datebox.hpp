#pragma once
#include "Container.hpp"
#include "../util/SimulationClock.hpp"
#include "StateTextButton.hpp"
#include "TextButton.hpp"
#include "Textfield.hpp"
#include <SFML/Graphics/Color.hpp>
#include <memory>
#include <vector>

namespace GUI {

class Datebox : public Container{
    Util::SimulationClock::time_point m_date;
    std::shared_ptr<Textfield> m_date_textfield, m_century_textfield, m_year_textfield, m_month_textfield;
    std::shared_ptr<Container> m_calendar_container;
    std::shared_ptr<StateTextButton<bool>> m_toggle_container_button;
    std::vector<std::shared_ptr<TextButton>> m_calendar_contents;

    void m_create_container();
    std::shared_ptr<TextButton> m_create_calendar_button(Container& c) const;
    void m_update_calendar();

public:
    explicit Datebox(Container& parent);

    void set_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color);
};

}