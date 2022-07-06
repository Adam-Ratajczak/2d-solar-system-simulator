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

#include <GL/glew.h>

#include <EssaGUI/gfx/SFMLWindow.hpp>
#include <EssaGUI/glwrapper/Color.hpp>
#include <EssaGUI/glwrapper/Vertex.hpp>
#include <EssaUtil/Vector3.hpp>
#include <initializer_list>
#include <iostream>
#include <vector>

class Sphere {
public:
    enum class DrawMode {
        Fancy,
        Full,
        Grid
    };
    // ctor/dtor
    Sphere(int sectorCount = 36, int stackCount = 18);
    ~Sphere() { }

    void set_color(Color color) { m_color = color; }
    void set_position(Vector3 pos) { m_pos = pos; }
    void set_radius(double radius) { m_radius = radius; }
    void set_draw_mode(DrawMode mode) { m_mode = mode; }
    void set_light_position(Vector3 pos) { m_light_position = pos; }

    void draw(GUI::SFMLWindow& window) const;

private:
    double m_radius {};
    unsigned m_sectors {};
    unsigned m_stacks {};
    DrawMode m_mode = DrawMode::Full;
    Vector3 m_pos;
    Vector3 m_light_position;
    sf::Color m_color;

    void gen_sphere();

    unsigned vertex_index(unsigned stack, unsigned sector) const;

    std::vector<Vertex> m_vertices;
    std::vector<unsigned> m_indices;
};
