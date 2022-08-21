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

#include <EssaEngine/3D/Sphere.hpp>
#include <EssaGUI/gfx/Window.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/Vector.hpp>
#include <initializer_list>
#include <iostream>
#include <vector>

class Sphere;

class SphereShader : public llgl::opengl::Shader {
public:
    SphereShader();

    virtual llgl::opengl::AttributeMapping attribute_mapping() const { return { 0, 100, 100, 100 }; };

    void set_sphere(Sphere const& sphere) { m_sphere = &sphere; }

private:
    Sphere const* m_sphere = nullptr;

    virtual void on_bind(llgl::opengl::ShaderScope& scope) const;
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

    void draw(SimulationView const&) const;

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
