#include "Sphere.hpp"

#include "../SimulationView.hpp"

#include <EssaUtil/Color.hpp>
#include <EssaUtil/DelayedInit.hpp>

#include <Essa/LLGL/Core/Transform.hpp>
#include <Essa/LLGL/OpenGL/Shader.hpp>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iomanip>
#include <iostream>

static char const VertexShader[] = R"~~~(// Planet VS
#version 330

layout (location = 0) in vec3 position;

uniform float radius;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 vertex;
out vec4 csVertex;

void main() {
    vertex = position;
    csVertex = projectionMatrix * (viewMatrix * (modelMatrix * vec4(vertex, 1)));
    gl_Position = csVertex;
}
)~~~";

static char const FragmentShader[] = R"~~~(// Planet FS
#version 330

uniform vec4 color;
uniform vec3 lightPosition;
uniform bool fancy;
uniform float radius;
uniform mat4 modelMatrix;

in vec3 vertex;
in vec4 csVertex;

void main() {
    if (!fancy) {
        gl_FragColor = color;
        return;
    }
    vec4 worldFragPos = modelMatrix * vec4(vertex, 1);
    if (radius > length(lightPosition - worldFragPos.xyz)) {
        // Light source is inside object. For now just fallback to flat mode.
        gl_FragColor = color;
        return;
    }
    float d = dot(normalize(-vertex), normalize(worldFragPos.xyz - lightPosition));
    gl_FragColor = vec4(d * color.rgb, 1);
}
)~~~";

void SphereShader::Uniforms::load_sphere(Sphere const& sphere) {
    m_color = sphere.color();
    m_radius = sphere.radius();
    if (sphere.mode() == Sphere::DrawMode::Fancy) {
        m_fancy = true;
        m_light_position = Util::Vector3f { sphere.light_position() };
    }
    else {
        m_fancy = false;
    }
}

std::string_view SphereShader::source(llgl::ShaderType type) const {
    switch (type) {
    case llgl::ShaderType::Vertex:
        return VertexShader;
    case llgl::ShaderType::Fragment:
        return FragmentShader;
    }
    ESSA_UNREACHABLE;
}

void Sphere::draw(Gfx::Painter& window, SimulationView const& sv) const {
    GUI::WorldDrawScope::verify();

    auto model = llgl::Transform {}.translate(Util::Cs::Vector3f::from_deprecated_vector(Util::Vector3f { m_position })).scale(m_radius);
    m_shader_uniforms.load_sphere(*this);
    m_shader_uniforms.set_transform(model.matrix(), sv.camera().view_matrix(), sv.projection().matrix());
    m_sphere.render(window.renderer(), m_shader, m_shader_uniforms);
}
