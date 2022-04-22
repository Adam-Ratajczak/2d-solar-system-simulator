#pragma once
#include "Container.hpp"
#include "../util/SimulationClock.hpp"
#include "StateTextButton.hpp"
#include "Textfield.hpp"
#include <SFML/Graphics/Color.hpp>
#include <memory>

namespace GUI {

class DateTextbox : public Container{
    Util::SimulationClock::time_point m_date;
    std::shared_ptr<Textfield> m_date_textfield;
    std::shared_ptr<StateTextButton<bool>> m_toggle_container_button;

public:
    explicit DateTextbox(Container& parent);

    void set_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color);
};

}