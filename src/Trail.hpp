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
    Vector3 m_offset;

    const sf::Color m_color;

public:
    Trail(size_t max_trail_size, sf::Color color);
    void draw();
    void push_back(Vector3 pos);
    void push_front(Vector3 pos);
    void pop_back();
    void reset();
    void set_offset(Vector3 offset) { m_offset = offset; }
    void recalculate_with_offset(Vector3 offset);
    void change_current(Vector3 pos);

private:
    friend std::ostream& operator<<(std::ostream&, Trail const&);
};
