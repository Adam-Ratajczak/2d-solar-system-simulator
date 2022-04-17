#pragma once
#include "Container.hpp"
#include "Date.hpp"

namespace GUI {

class DateTextbox : public Container{
    Date m_date;
public:
    explicit DateTextbox(Container& parent);
};

}