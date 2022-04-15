#pragma once

#include "SimulationView.hpp"
#include "glwrapper/Vertex.hpp"
#include "math/Vector3.hpp"
#include <SFML/Graphics.hpp>
#include <list>

// TODO: Strip this class of all display-related things.
class Trail {
    std::vector<Vertex> m_display_trail;
    size_t m_display_trail_append_offset = 0;
    size_t m_display_trail_length = 0;
    
public:
    Trail(size_t max_trail_size);
    void draw();
    void update_trail(Vector3 pos, const sf::Color& m_color);
};
