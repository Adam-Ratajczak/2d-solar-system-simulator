#pragma once

#include "SimulationView.hpp"
#include "glwrapper/Vertex.hpp"
#include "math/Vector3.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <list>

class Trail {
    std::vector<Vertex> m_vertexes;
    int m_append_offset = 1;
    int m_length = 0;
    int m_seconds_per_tick = 0, m_total_seconds = 0;
    Vector3 m_offset;

    const sf::Color m_color;

public:
    Trail(size_t max_trail_size, sf::Color color);
    void draw();
    void push_back(Vector3 pos);
    void reset();
    void set_offset(Vector3 offset) { m_offset = offset; }
    void recalculate_with_offset(Vector3 offset);
    void change_current(Vector3 pos);
    void set_new_tickrate(unsigned sec){m_seconds_per_tick = sec;}

private:
    friend std::ostream& operator<<(std::ostream&, Trail const&);
};
