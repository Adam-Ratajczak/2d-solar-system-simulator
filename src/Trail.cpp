#include "Trail.hpp"

#include "Object.hpp"
#include "glwrapper/Helpers.hpp"
#include "math/Transform.hpp"
#include "math/Vector3.hpp"
#include <GL/gl.h>
#include <SFML/Graphics.hpp>
#include <iostream>

Trail::Trail(size_t max_trail_size, sf::Color color)
    // FIXME: Engine shouldn't limit user
    : m_vertexes((max_trail_size == 0) ? 1000 : max_trail_size)
    , m_color(color) { }

void Trail::push_back(Vector3 pos) {
    m_vertexes[m_append_offset] = Vertex { .position = pos / AU, .color = m_color };
    m_append_offset++;
    if (m_length < m_vertexes.size())
        m_length++;
    if (m_append_offset == m_vertexes.size()) {
        m_vertexes[0] = Vertex { .position = pos / AU, .color = m_color };
        m_append_offset = 1;
    }
}

void Trail::push_front(Vector3 pos) {
    m_append_offset--;
    if (m_append_offset < 0)
        m_append_offset = m_length;
    m_vertexes[m_append_offset] = Vertex { .position = pos / AU, .color = m_color };
}

void Trail::pop_back() {
    if (m_length > 0)
        m_length--;
}

void Trail::reset() {
    m_append_offset = 1;
    m_length = 0;
}

void Trail::recalculate_with_offset(Vector3 offset) {
    if (m_offset == offset)
        return;
    for (size_t s = 0; s < m_length; s++)
        m_vertexes[s].position = m_vertexes[s].position - m_offset + offset;
    m_offset = offset;
}

void Trail::draw() {
    if (m_offset != Vector3()) {
        glPushMatrix();
        Transform::translation(m_offset / AU).gl_mult();
    }

    if (m_length != m_vertexes.size()) {
        GL::draw_vertices(GL_LINE_STRIP, { m_vertexes.data() + 1, static_cast<size_t>(m_append_offset - 1) });
    }
    else {
        GL::draw_vertices(GL_LINE_STRIP, { m_vertexes.data(), static_cast<size_t>(m_append_offset) });
        GL::draw_vertices(GL_LINE_STRIP, { m_vertexes.data() + m_append_offset, static_cast<size_t>(m_length - m_append_offset) });
    }

    if (m_offset != Vector3())
        glPopMatrix();
}

std::ostream& operator<<(std::ostream& out, Trail const& trail) {
    out << "Trail(" << trail.m_length << "): ";
    for (size_t s = 0; auto& t : trail.m_vertexes) {
        out << t.position << ", ";
        s++;
        if (s > trail.m_length)
            break;
    }
    return out;
}
