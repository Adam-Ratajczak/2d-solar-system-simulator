#pragma once

#include "SimulationView.hpp"
#include "glwrapper/Vertex.hpp"
#include "math/Vector3.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <list>

class Trail {
    std::vector<Vertex> m_display_trail;
    int m_display_trail_append_offset = 0;
    int m_display_trail_length = 0;

    const sf::Color m_color;
    
public:
    Trail(size_t max_trail_size, sf::Color color);
    void draw();
    void push_back(Vector3 pos);
    void push_front(Vector3 pos);
};
