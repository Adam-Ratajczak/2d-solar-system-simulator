#include "Sphere.hpp"
#include "../SimulationView.hpp"
#include "../math/Transform.hpp"
#include "Color.hpp"
#include "Helpers.hpp"

#include <GL/gl.h>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>

const int MIN_SECTOR_COUNT = 3;
const int MIN_STACK_COUNT = 2;

Sphere::Sphere(double radius, int sectors, int stacks)
    : m_radius(radius)
    , m_sectors(sectors)
    , m_stacks(stacks) {
    gen_sphere();
}

void Sphere::gen_sphere() {
    float delta_stack_angle = M_PI / m_stacks;
    float delta_sector_angle = 2 * M_PI / m_sectors;
    for (unsigned stack = 0; stack < m_stacks; stack++) {
        for (unsigned sector = 0; sector < m_sectors; sector++) {
            auto stack_angle = stack * delta_stack_angle;
            auto sector_angle = sector * delta_sector_angle;
            Vector3 point_position { 1 * std::cos(sector_angle) * std::sin(stack_angle),
                1 * std::sin(sector_angle) * std::sin(stack_angle),
                1 * std::cos(stack_angle) };
            m_vertices.push_back(Vertex { .position = point_position });
            // std::cout << "   --- " << std::sin(sector_angle) << ";" << std::cos(sector_angle) << " ;; " << std::sin(stack_angle) << ";" << std::cos(stack_angle) << std::endl;
            // std::cout << point_position.x << "," << point_position.y << "," << point_position.z << " @ " << stack_angle << "," << sector_angle << std::endl;

            m_indices.push_back(vertex_index(stack, sector));
            m_indices.push_back(vertex_index(stack, sector + 1));
            m_indices.push_back(vertex_index(stack + 1, sector + 1));

            m_indices.push_back(vertex_index(stack + 1, sector + 1));
            m_indices.push_back(vertex_index(stack + 1, sector));
            m_indices.push_back(vertex_index(stack, sector));
        }
    }
    m_vertices.push_back(Vertex { .position { 0, 0, -1 } });
    assert(m_vertices.size() == m_sectors * m_stacks + 1);

    // std::cout << "----------------------" << std::endl;
    // for (auto& i : m_indices)
    //     std::cout << m_vertices[i].position << std::endl;

    change_colours();
}

unsigned Sphere::vertex_index(unsigned stack, unsigned sector) const {
    assert(stack <= m_stacks && sector <= m_sectors);
    if (stack == m_stacks)
        return m_stacks * m_sectors;
    return stack * m_sectors + (sector % m_sectors);
}

void Sphere::set_color(Color color) {
    for (auto& vert : m_vertices)
        vert.color = color;
}

void Sphere::draw() const {
    WorldDrawScope::verify();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    Transform::translation(m_pos).gl_mult();
    Transform::scale({ m_radius, m_radius, m_radius }).gl_mult();
    GL::draw_indexed_vertices(GL_TRIANGLES, m_vertices, m_indices);
    glPopMatrix();
}

void Sphere::change_colours() {
    for (const auto& i : m_indices) {
        m_vertices[i].color *= (float)i / m_indices.size();
    }
}
