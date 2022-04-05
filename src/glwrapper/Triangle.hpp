#pragma once

#include "Color.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
class Triangle{
    Vertex v1, v2, v3;

public:
    Triangle(Vertex _v1, Vertex _v2, Vertex _v3)
    : v1(_v1)
    , v2(_v2)
    , v3(_v3){}

    void draw(sf::RenderWindow& window) const;
};
