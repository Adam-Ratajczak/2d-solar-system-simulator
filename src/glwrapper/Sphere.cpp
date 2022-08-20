#include "Sphere.hpp"

#include "../SimulationView.hpp"

#include <EssaUtil/Color.hpp>
#include <EssaUtil/DelayedInit.hpp>
#include <GL/gl.h>
#include <LLGL/OpenGL/Shader.hpp>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iomanip>
#include <iostream>

Util::DelayedInit<llgl::opengl::Program> s_program;
bool m_shader_loaded = false;

static char const s_vertex_shader[] = R"~~~(// Planet VS
#version 330

layout (location = 0) in vec4 position;

uniform vec4 translation;
uniform float radius;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec4 vertex;
out vec4 csVertex;

void main() {
    vertex = position;
    csVertex = projectionMatrix * (viewMatrix * (modelMatrix * vertex));
    gl_Position = csVertex;
}
)~~~";

static char const s_fragment_shader[] = R"~~~(// Planet FS
#version 330

uniform vec4 color;
uniform vec3 lightPosition;
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
    if (radius > length(lightPosition - worldFragPos.xyz)) {
        // Light source is inside object. For now just fallback to flat mode.
        gl_FragColor = color;
        return;
    }
    float d = dot(normalize(-vertex).xyz, normalize(worldFragPos.xyz - lightPosition));
    gl_FragColor = vec4(d * color.rgb, 1);
}
)~~~";

llgl::opengl::Program& sphere_shader() {
    if (!m_shader_loaded) {
        m_shader_loaded = true;
        std::cout << "Sphere: Loading planet shader" << std::endl;
        auto objects = {
            llgl::opengl::ShaderObject { s_vertex_shader, llgl::opengl::ShaderObject::Type::Vertex },
            llgl::opengl::ShaderObject { s_fragment_shader, llgl::opengl::ShaderObject::Type::Fragment }
        };
        s_program.construct(objects);
        if (!s_program->valid()) {
            std::cout << "Failed to load planet shader. Fancy mode will not work." << std::endl;
            m_shader_loaded = false;
        }
    }
    return *s_program;
}

SphereShader::SphereShader()
    : Shader(sphere_shader()) {
}

void SphereShader::on_bind(llgl::opengl::ShaderScope& scope) const {
    scope.set_uniform("color", m_sphere->color());
    scope.set_uniform("translation", Util::Vector3f { m_sphere->position() });
    scope.set_uniform("radius", static_cast<float>(m_sphere->radius()));
    if (m_sphere->mode() == Sphere::DrawMode::Fancy) {
        scope.set_uniform("fancy", true);
        scope.set_uniform("lightPosition", Util::Vector3f { m_sphere->light_position() });
    }
    else {
        scope.set_uniform("fancy", false);
    }
}

void Sphere::draw(SimulationView const& sv) const {
    WorldDrawScope::verify();

    // HACK: Normally (e.g when using EssaGUI's draw_vertices) EssaGUI
    // makes sure that the view is applied. It's not done when doing
    // direct LLGL rendering, so we need to do this manually here.
    sv.window().renderer().apply_projection(sv.projection());

    auto model = llgl::Transform {}.translate(Util::Vector3f { m_position }).scale(m_radius);
    m_shader.set_sphere(*this);
    sv.window().renderer().render_object(m_sphere, { .shader = &m_shader, .model_matrix = model.matrix(), .view_matrix = sv.camera_transform().matrix() });
}
