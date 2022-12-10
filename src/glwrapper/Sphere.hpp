///////////////////////////////////////////////////////////////////////////////
// Sphere.h
// ========
// Sphere for OpenGL with (radius, sectors, stacks)
// The min number of sectors is 3 and The min number of stacks are 2.
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2017-11-01
// UPDATED: 2020-05-20
///////////////////////////////////////////////////////////////////////////////

// (But heavily modified)

#pragma once

#include "../SimulationView.hpp"

#include <Essa/Engine/3D/Sphere.hpp>
#include <Essa/GUI/Graphics/Window.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Vector.hpp>
#include <Essa/LLGL/OpenGL/ShaderBases/Transform.hpp>
#include <initializer_list>
#include <iostream>
#include <vector>

class Sphere;

class SphereShader : public llgl::Shader
    , public llgl::ShaderBases::Transform {
public:
    using Vertex = Essa::Model::Vertex;

    auto uniforms() {
        return llgl::UniformList { m_fancy, m_radius, m_color, m_light_position }
        + llgl::ShaderBases::Transform::uniforms();
    }

    std::string_view source(llgl::ShaderType type) const;

    void load_sphere(Sphere const& sphere);

private:
    llgl::Uniform<bool> m_fancy { "fancy" };
    llgl::Uniform<float> m_radius { "radius" };
    llgl::Uniform<Util::Colorf> m_color { "color" };
    llgl::Uniform<Util::Vector3f> m_light_position { "lightPosition" };
};

class Sphere {
public:
    enum class DrawMode {
        Fancy,
        Full,
        Grid
    };

    void set_color(Util::Color color) { m_color = color; }
    void set_position(Util::Vector3d pos) { m_position = pos; }
    void set_radius(double radius) { m_radius = radius; }
    void set_draw_mode(DrawMode mode) { m_mode = mode; }
    void set_light_position(Util::Vector3d pos) { m_light_position = pos; }

    void draw(Gfx::Painter& window, SimulationView const&) const;

    double radius() const { return m_radius; }
    Util::Color color() const { return m_color; }
    Util::Vector3d position() const { return m_position; }
    DrawMode mode() const { return m_mode; }
    Util::Vector3d light_position() const { return m_light_position; }

private:
    DrawMode m_mode = DrawMode::Full;
    Util::Vector3d m_position;
    Util::Vector3d m_light_position;
    double m_radius {};
    Util::Color m_color;
    mutable SphereShader m_shader;
    Essa::Sphere m_sphere;
};
