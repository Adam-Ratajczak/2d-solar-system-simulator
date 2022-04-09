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

#pragma once

#include "../Vector3.hpp"
#include "Color.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <initializer_list>
#include <vector>

class Sphere {
public:
    // ctor/dtor
    Sphere(double radius, int sectorCount = 36, int stackCount = 18);
    ~Sphere() { }

    void set_color(Color color);
    void set_position(Vector3 pos) { m_pos = pos; }

    void draw() const;

private:
    double m_radius {};
    unsigned m_sectors {};
    unsigned m_stacks {};
    Vector3 m_pos;

    void gen_sphere();
    void change_colours();

    unsigned vertex_index(unsigned stack, unsigned sector) const;

    std::vector<Vertex> m_vertices;
    std::vector<unsigned> m_indices;
};
