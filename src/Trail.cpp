#include "Trail.hpp"

#include "Object.hpp"
#include "glwrapper/Helpers.hpp"
#include "math/Transform.hpp"
#include "math/Vector3.hpp"
#include <GL/gl.h>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

static constexpr auto TrailMinStep = 60 * 60 * 12;
static constexpr size_t TrailMaxAllocatedSpace = 0xfffff;

Trail::Trail(size_t max_trail_size, sf::Color color)
    : m_color(color) {

    // Trail must have at least the beginning, the end and the "glue" vertex
    if (max_trail_size < 3) {
        std::cout << "FIXME: Trail: Something is wrong! Trying to create trail of size < 3" << std::endl;
        max_trail_size = 3;
    }

    if (max_trail_size > TrailMaxAllocatedSpace) {
        std::cout << "FIXME: Trail: Something is wrong! Trying to create trail of size " << max_trail_size << ", max is " << TrailMaxAllocatedSpace << std::endl;
        max_trail_size = TrailMaxAllocatedSpace;
    }
    m_vertexes.resize(max_trail_size);
    m_length++;
}

std::pair<Vector3, Vector3> Trail::m_get_last_two_entries() const {
    int i1 = m_append_offset;
    int i2 = m_append_offset - 1;

    if (i1 < 0)
        i1 += m_vertexes.size();

    if (i2 < 0)
        i2 += m_vertexes.size();

    return std::make_pair(m_vertexes[i1].position, m_vertexes[i2].position);
}

void Trail::push_back(Vector3 pos) {
    // Ensure that the trail always has the beginning
    if (m_length == 1) {
        assert(m_append_offset == 1);
        m_vertexes[m_append_offset] = Vertex { .position = pos / AU, .color = m_color };
        m_append_offset++;
        m_length++;
    }

    if (m_length >= 3 && m_append_offset != 0) {
        if (std::fabs(std::atan2(pos.y, pos.x) - last_xy_angle) <= 2 * M_PI / m_vertexes.size() && std::fabs(std::atan2(pos.y, pos.z) - last_yz_angle) <= 2 * M_PI / m_vertexes.size()) {
            change_current(pos);
            return;
        }
    }
    m_vertexes[m_append_offset] = Vertex { .position = pos / AU, .color = m_color };

    m_append_offset++;
    if (m_length < m_vertexes.size())
        m_length++;
    if (m_append_offset == m_vertexes.size()) {
        m_vertexes[0] = Vertex { .position = pos / AU, .color = m_color };
        m_append_offset = 1;
    }

    last_xy_angle = std::atan2(pos.y, pos.x);
    last_yz_angle = std::atan2(pos.y, pos.z);
}

void Trail::reset() {
    m_append_offset = 1;
    m_length = 1;
}

void Trail::recalculate_with_offset(Vector3 offset) {
    if (m_offset == offset)
        return;
    for (size_t s = 0; s < m_length; s++)
        m_vertexes[s].position = m_vertexes[s].position + m_offset / AU - offset / AU;
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

void Trail::change_current(Vector3 pos) {
    assert(m_length > 0);
    if (m_append_offset == 1)
        m_vertexes[m_length - 1].position = pos / AU;
    m_vertexes[m_append_offset - 1].position = pos / AU;
}

std::ostream& operator<<(std::ostream& out, Trail const& trail) {
    out << "Trail(" << trail.m_length << " a@ " << trail.m_append_offset << ")";
    return out;
}
