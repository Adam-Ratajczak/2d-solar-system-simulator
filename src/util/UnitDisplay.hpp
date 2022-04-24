#pragma once

#include <SFML/System.hpp>

namespace Util {

enum class Quantity {
    Length,
    Mass,
    Velocity,
    Time,

    None,
};

sf::String to_exponent_string(double value);

struct UnitValue {
    sf::String value;
    sf::String unit;
};

UnitValue unit_display(double value, Quantity);

}
