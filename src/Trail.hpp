#pragma once

#include "Vector2.hpp"
#include "gui/SimulationView.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <list>
class Trail {
    struct TrailEntry {
        Vector2 pos;
        Vector2 vel;
    };
    std::list<TrailEntry> m_trail;
    Vector2 m_prev_offset, m_prev_window_size;
    double m_prev_zoom;
    bool m_reverse_con = true, m_refresh = true;
    size_t m_append_index = 0;
    size_t m_max_trail_size = 0;
    sf::VertexArray m_trail_vertexbuffer;

public:
    Trail(size_t max_trail_size);
    void push_back(Vector2 pos, Vector2 vel);
    void push_front(Vector2 pos, Vector2 vel);
    void pop_front();
    void draw(SimulationView const& view, sf::Color color);
    void update_trail(SimulationView const& view, const sf::Color& m_color);

    bool reverse_path(bool reverse, Object* obj);

    unsigned size() const { return m_trail.size(); }
};
