#pragma once
#include "Container.hpp"
#include "../util/SimulationClock.hpp"

namespace GUI {

class DateTextbox : public Container{
    Util::SimulationClock m_date;
public:
    explicit DateTextbox(Container& parent);
};

}