#pragma once

#include <SFML/System.hpp>
#include <string>

class Widget;

struct Tooltip {
    std::string text;
    Widget* owner = nullptr;
    sf::Vector2f position;

    Tooltip(std::string t, Widget* o, sf::Vector2f p)
    : text(std::move(t)), owner(o), position(p) {}
};
