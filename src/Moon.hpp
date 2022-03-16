#pragma once

#include "Object.hpp"
class Moon : public Object{
public:
    Moon(double mass, double radius, Vector2 pos, Vector2 vel, sf::Color color, std::string name, unsigned tres);

    void update(std::list<Moon>& moon_list);
};