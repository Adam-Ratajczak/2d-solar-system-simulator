#pragma once

#include "Moon.hpp"
#include "Object.hpp"
#include "Vector2.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <list>

class Planet : public Object{
    friend class World;
protected:
    std::list<Moon> moon_list;
public:
    Planet(double, double, Vector2, Vector2, sf::Color, std::string, unsigned);

    void update(std::list<Planet>&);
};

inline bool operator==(const Planet& a, const Planet& b){return &a == &b;}
