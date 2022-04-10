#include "Trail.hpp"
#include "Object.hpp"
#include "math/Vector3.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>

Trail::Trail(size_t max_trail_size)
    : m_max_trail_size(max_trail_size)
    , m_trail_vertexbuffer(sf::Lines, m_max_trail_size * 2) { }

void Trail::push_back(const Vector3& pos) {
    m_trail.push_back(pos);
}

void Trail::push_front(const Vector3& pos) {
    m_trail.push_front(pos);
}

void Trail::pop_front() {
    m_trail.pop_front();
}

void Trail::update_trail(SimulationView const& view, const sf::Color& m_color) {
    auto color = m_color;
    color.a = 128;

    // FIXME: This assumes that max trail size doesn't change.
    // TODO: Rewrite to raw OpenGL.
    /*
    m_trail_vertexbuffer[m_append_index] = sf::Vertex(view.world_to_screen(*(--(--(m_trail.end())))), color);
    m_trail_vertexbuffer[m_append_index + 1] = sf::Vertex(view.world_to_screen(m_trail.back()), color);
    m_append_index += 2;
    if (m_append_index >= m_trail_vertexbuffer.getVertexCount() - 1)
        m_append_index = 0;
        */
}

void Trail::m_recalculate(SimulationView const& view, sf::Color m_color) {
    // TODO: Rewrite to raw OpenGL.
    /*
    auto color = m_color;
    color.a = 128;

    // FIXME: Maybe add a way to invalidate trail from the outside
    if (m_prev_offset != view.offset() || m_prev_zoom != view.scale() || m_prev_window_size != view.window().getSize()) {
        size_t index = 0;

        // FIXME: VERY HACKY FIX THIS ASAP
        m_trail_vertexbuffer.clear();
        m_trail_vertexbuffer.resize(m_max_trail_size * 2);
        for (auto it = m_trail.begin(); it != --m_trail.end();) {
            if (index + 1 >= m_trail_vertexbuffer.getVertexCount())
                break;
            auto previous_it = it++;
            m_trail_vertexbuffer[index++] = sf::Vertex(view.world_to_screen(*previous_it), color);
            m_trail_vertexbuffer[index++] = sf::Vertex(view.world_to_screen(*it), color);
        }
    }

    m_prev_offset = view.offset();
    m_prev_zoom = view.scale();
    m_prev_window_size = view.window().getSize();
    */
}

void Trail::draw(SimulationView const& view, sf::Color m_color) {
    auto& target = view.window();

    m_recalculate(view, m_color);

    target.draw(m_trail_vertexbuffer);
}
