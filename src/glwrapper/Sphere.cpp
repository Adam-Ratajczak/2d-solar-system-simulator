#include "Sphere.hpp"

#include "../SimulationView.hpp"
#include "../math/Transform.hpp"

#include <EssaUtil/Color.hpp>
#include <GL/gl.h>
#include <SFML/Graphics/Glsl.hpp>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iomanip>
#include <iostream>

const int MIN_SECTOR_COUNT = 3;
const int MIN_STACK_COUNT = 2;

sf::Shader s_shader;
bool m_shader_loaded = false;

static char const s_vertex_shader[] = R"~~~(// Planet VS
#version 330

layout (location = 0) in vec4 position;

uniform vec4 translation;
uniform float radius;
uniform mat4 modelMatrix;
uniform mat4 worldViewMatrix;
uniform mat4 projectionMatrix;

out vec4 vertex;
out vec4 csVertex;

void main() {
    vertex = position;
    csVertex = projectionMatrix * (worldViewMatrix * (modelMatrix * vertex));
    gl_Position = csVertex;
}
)~~~";

static char const s_fragment_shader[] = R"~~~(// Planet FS
#version 330

uniform vec4 color;
uniform vec4 lightPosition;
uniform bool fancy;
uniform vec4 translation;
uniform float radius;
uniform mat4 modelMatrix;

in vec4 vertex;
in vec4 csVertex;

void main() {
    if (!fancy) {
        gl_FragColor = color;
        return;
    }
    vec4 worldFragPos = modelMatrix * vertex;
    if (radius > length(lightPosition - worldFragPos)) {
        // Light source is inside object. For now just fallback to flat mode.
        gl_FragColor = color;
        return;
    }
    float d = dot(normalize(-vertex), normalize(worldFragPos - lightPosition));
    gl_FragColor = vec4(d * color.rgb, 1);
}
)~~~";

Sphere::Sphere(int sectors, int stacks)
    : m_sectors(sectors)
    , m_stacks(stacks) {
    gen_sphere();
    if (!m_shader_loaded) {
        m_shader_loaded = true;
        std::cout << "Sphere: Loading planet shader" << std::endl;
        if (!s_shader.loadFromMemory(s_vertex_shader, s_fragment_shader)) {
            std::cout << "Failed to load planet shader. Fancy mode will not work." << std::endl;
            m_shader_loaded = false;
        }
    }
}

void Sphere::gen_sphere() {
    float delta_stack_angle = M_PI / m_stacks;
    float delta_sector_angle = 2 * M_PI / m_sectors;
    for (unsigned stack = 0; stack < m_stacks; stack++) {
        for (unsigned sector = 0; sector < m_sectors; sector++) {
            auto stack_angle = stack * delta_stack_angle;
            auto sector_angle = sector * delta_sector_angle;
            Util::Vector3f point_position { 1 * std::cos(sector_angle) * std::sin(stack_angle),
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

void Sphere::draw(GUI::SFMLWindow& window) const {
    WorldDrawScope::verify();

    auto model_matrix = Transform::scale({ m_radius, m_radius, m_radius }) * Transform::translation(m_pos);
    GUI::SFMLWindow::ModelScope scope(window, model_matrix);

    s_shader.setUniform("color", sf::Glsl::Vec4 { m_color.r / 255.f, m_color.g / 255.f, m_color.b / 255.f, m_color.a / 255.f });
    s_shader.setUniform("translation", sf::Glsl::Vec4 { static_cast<float>(m_pos.x()), static_cast<float>(m_pos.y()), static_cast<float>(m_pos.z()), 0 });
    s_shader.setUniform("radius", (float)m_radius);
    if (m_mode == DrawMode::Fancy) {
        s_shader.setUniform("fancy", true);
        s_shader.setUniform("lightPosition", sf::Glsl::Vec4 { static_cast<float>(m_light_position.x()), static_cast<float>(m_light_position.y()), static_cast<float>(m_light_position.z()), 1 });
    }
    else {
        s_shader.setUniform("fancy", false);
    }

    WorldDrawScope::current()->apply_uniforms(s_shader);
    window.set_shader(&s_shader);
    if (m_mode == DrawMode::Grid)
        window.draw_indexed_vertices(GL_LINES, m_vertices, m_indices);
    else
        window.draw_indexed_vertices(GL_TRIANGLES, m_vertices, m_indices);
    window.set_shader(nullptr);
}
