#include "Trail.hpp"
#include "Object.hpp"
#include "Vector3.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Window/Mouse.hpp>
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

void Trail::m_recalculate(SimulationView const& view, sf::Color m_color){
    auto color = m_color;
    color.a = 128;

    // FIXME: Maybe add a way to invalidate trail from the outside
    if (m_prev_zoom != view.scale() || m_prev_window_size != view.window().getSize()) {
        size_t index = 0;

        while(index < m_trail_vertexbuffer.getVertexCount()) {
            // (v - Vector3(window().getSize() / 2u)) / scale() + m_offset
            m_trail_vertexbuffer[index].position = view.world_to_screen((m_trail_vertexbuffer[index].position - Vector3(m_prev_window_size / 2u)) / m_prev_zoom + m_prev_offset);
            index++;
        }
    }else if(m_prev_offset != view.offset()){
        size_t index = 0;
        auto delta_pos = sf::Mouse::getPosition() - m_prev_mouse_pos;

        while(index < m_trail_vertexbuffer.getVertexCount()) {
            m_trail_vertexbuffer[index].position = m_trail_vertexbuffer[index].position + delta_pos;
            index++;
        }
    }

    m_prev_offset = view.offset();
    m_prev_zoom = view.scale();
    m_prev_window_size = view.window().getSize();
    m_prev_mouse_pos = sf::Mouse::getPosition();
}

void Trail::draw(SimulationView const& view, sf::Color m_color) {
    auto& target = view.window();
    
    target.draw(m_trail_vertexbuffer);

    m_recalculate(view, m_color);
}
