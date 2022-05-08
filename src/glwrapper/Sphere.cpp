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

sf::Shader s_shader;
bool m_shader_loaded = false;

static char const s_vertex_shader[] = R"~~~(
#version 110

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
)~~~";

static char const s_fragment_shader[] = R"~~~(
#version 110

uniform vec4 color;

void main()
{
    gl_FragColor = color;
}
)~~~";

Sphere::Sphere(int sectors, int stacks)
    : m_sectors(sectors)
    , m_stacks(stacks) {
    gen_sphere();
    if (!m_shader_loaded) {
        m_shader_loaded = true;
        std::cout << "Loading shader" << std::endl;
        s_shader.loadFromMemory(s_vertex_shader, sf::Shader::Vertex);
        s_shader.loadFromMemory(s_fragment_shader, sf::Shader::Fragment);
    }
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
}

unsigned Sphere::vertex_index(unsigned stack, unsigned sector) const {
    assert(stack <= m_stacks && sector <= m_sectors);
    if (stack == m_stacks)
        return m_stacks * m_sectors;
    return stack * m_sectors + (sector % m_sectors);
}

void Sphere::draw() const {
    WorldDrawScope::verify();

    sf::Shader::bind(&s_shader);
    s_shader.setUniform("color", sf::Glsl::Vec4 { m_color.r / 255.f, m_color.g / 255.f, m_color.b / 255.f, m_color.a / 255.f });
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    Transform::translation(m_pos).gl_mult();
    Transform::scale({ m_radius, m_radius, m_radius }).gl_mult();
    if (m_mode == DrawMode::Full)
        GL::draw_indexed_vertices(GL_TRIANGLES, m_vertices, m_indices);
    else if (m_mode == DrawMode::Grid)
        GL::draw_indexed_vertices(GL_LINES, m_vertices, m_indices);
    sf::Shader::bind(nullptr);
    glPopMatrix();
}
