#include "Trail.hpp"
#include "Object.hpp"
#include "Vector3.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <iostream>

Trail::Trail(size_t max_trail_size)
    : m_max_trail_size(max_trail_size)
    , m_trail_vertexbuffer(sf::Lines, m_max_trail_size * 2) { }

void Trail::push_back(Vector3 pos, Vector3 vel) {
    m_trail.push_back({ pos, vel });
}

void Trail::push_front(Vector3 pos, Vector3 vel) {
    m_trail.push_front({ pos, vel });
}

void Trail::pop_front() {
    m_trail.pop_front();
}

void Trail::update_trail(SimulationView const& view, const sf::Color& m_color) {
    auto color = m_color;
    color.a = 128;

    // FIXME: This assumes that max trail size doesn't change.
    m_trail_vertexbuffer[m_append_index] = sf::Vertex(view.world_to_screen((--(--(m_trail.end())))->pos), color);
    m_trail_vertexbuffer[m_append_index + 1] = sf::Vertex(view.world_to_screen(m_trail.back().pos), color);
    m_append_index += 2;
    if (m_append_index >= m_trail_vertexbuffer.getVertexCount() - 1)
        m_append_index = 0;
}

void Trail::draw(SimulationView const& view, sf::Color m_color) {
    auto& target = view.window();
    auto color = m_color;
    color.a = 128;

    // FIXME: Maybe add a way to invalidate trail from the outside
    if (m_prev_offset != view.offset() || m_prev_zoom != view.scale() || m_prev_window_size != view.window().getSize() || m_refresh) {
        // Fully recalculate trail (very heavy for long trails)
        size_t index = 0;

        // FIXME: VERY HACKY FIX THIS ASAP
        m_trail_vertexbuffer.clear();
        m_trail_vertexbuffer.resize(m_max_trail_size * 2);
        for (auto it = m_trail.begin(); it != --m_trail.end();) {
            if (index + 1 >= m_trail_vertexbuffer.getVertexCount())
                break;
            auto previous_it = it++;
            m_trail_vertexbuffer[index++] = sf::Vertex(view.world_to_screen(previous_it->pos), color);
            m_trail_vertexbuffer[index++] = sf::Vertex(view.world_to_screen(it->pos), color);
        }
        m_append_index = index;
    }

    target.draw(m_trail_vertexbuffer);

    m_prev_offset = view.offset();
    m_prev_zoom = view.scale();
    m_prev_window_size = view.window().getSize();

    m_refresh = false;
}

bool Trail::reverse_path(bool reverse, Object* obj) {
    if (reverse && m_trail.size() > 0 && m_reverse_con) {
        obj->m_pos = m_trail.back().pos;
        obj->m_vel = -m_trail.back().vel;
        m_trail.pop_back();

        m_refresh = true;
        return 1;
    }

    if (reverse)
        m_reverse_con = 0;

    if (!reverse && m_trail.size() > 0 && !m_reverse_con) {
        obj->m_pos = m_trail.back().pos;
        obj->m_vel = -m_trail.back().vel;
        m_trail.pop_back();

        m_refresh = true;
        return 1;
    }

    if (!reverse)
        m_reverse_con = 1;

    return 0;
}
