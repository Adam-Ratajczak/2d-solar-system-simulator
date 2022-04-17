#include "Trail.hpp"
#include "Object.hpp"
#include "glwrapper/Helpers.hpp"
#include "math/Vector3.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>

Trail::Trail(size_t max_trail_size, sf::Color color)
    : m_display_trail((max_trail_size == 0) ? 1000 : max_trail_size)
    , m_color(color) { }

void Trail::push_back(Vector3 pos) {
    m_display_trail[m_display_trail_append_offset] = Vertex { .position = pos / AU, .color = m_color };
    m_display_trail_append_offset++;
    if (m_display_trail_length < m_display_trail.size())
        m_display_trail_length++;
    if (m_display_trail_append_offset >= m_display_trail.size())
        m_display_trail_append_offset = 0;
}

void Trail::push_front(Vector3 pos) {
    m_display_trail_append_offset--;
    if (m_display_trail_append_offset < 0)
        m_display_trail_append_offset = m_display_trail_length - 1;
    m_display_trail[m_display_trail_append_offset] = Vertex { .position = pos / AU, .color = m_color };
}

void Trail::pop_back() {
    if (m_display_trail_length > 0)
        m_display_trail_length--;
}

void Trail::reset() {
    m_display_trail_append_offset = 0;
    m_display_trail_length = 0;
}

void Trail::draw() {
    GL::draw_vertices(GL_LINE_STRIP, { m_display_trail.data(), static_cast<size_t>(m_display_trail_append_offset) });
    GL::draw_vertices(GL_LINE_STRIP, { m_display_trail.data() + m_display_trail_append_offset, static_cast<size_t>(m_display_trail_length - m_display_trail_append_offset) });
}

std::ostream& operator<<(std::ostream& out, Trail const& trail) {
    out << "Trail(" << trail.m_display_trail_length << "): ";
    for (size_t s = 0; auto& t : trail.m_display_trail) {
        out << t.position << ", ";
        s++;
        if (s > trail.m_display_trail_length)
            break;
    }
    return out;
}
