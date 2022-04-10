#pragma once

#include "math/Vector3.hpp"
#include "gui/SimulationView.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <list>

// TODO: Strip this class of all display-related things.
class Trail {
    std::list<Vector3> m_trail;
    Vector3 m_prev_offset, m_prev_window_size;
    double m_prev_zoom {};
    size_t m_append_index = 0;
    size_t m_max_trail_size = 0;
    sf::VertexArray m_trail_vertexbuffer;
    void m_recalculate(SimulationView const& view, sf::Color m_color);

public:
    Trail(size_t max_trail_size);
    void push_back(const Vector3 &pos);
    void push_front(const Vector3 &pos);
    void pop_front();
    void draw(SimulationView const& view, sf::Color color);
    void update_trail(SimulationView const& view, const sf::Color& m_color);

    unsigned size() const { return m_trail.size(); }
};
