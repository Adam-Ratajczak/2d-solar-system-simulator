#include "Trail.hpp"
#include "Object.hpp"
#include "glwrapper/Helpers.hpp"
#include "math/Vector3.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>

Trail::Trail(size_t max_trail_size)
    : m_display_trail(max_trail_size){}

void Trail::update_trail(Vector3 pos, const sf::Color& m_color) {
    m_display_trail[m_display_trail_append_offset] = Vertex { .position = pos / AU, .color = m_color };
    m_display_trail_append_offset++;
    if (m_display_trail_length < m_display_trail.size())
        m_display_trail_length++;
    if (m_display_trail_append_offset >= m_display_trail.size())
        m_display_trail_append_offset = 0;
}

void Trail::draw() {

    GL::draw_vertices(GL_LINE_STRIP, { m_display_trail.data(), m_display_trail_append_offset });
    GL::draw_vertices(GL_LINE_STRIP, { m_display_trail.data() + m_display_trail_append_offset, m_display_trail_length - m_display_trail_append_offset });

}
