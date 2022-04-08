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

#include <SFML/Graphics/RenderWindow.hpp>
#include <initializer_list>
#include <vector>
#include "../Vector3.hpp"
#include "Color.hpp"

class Sphere
{
public:
    // ctor/dtor
    Sphere(Vector3 pos, double radius, int sectorCount = 36, int stackCount = 18);
    ~Sphere() {}

    void set_colors(std::initializer_list<std::pair<Color, int>> list);

    void draw() const;
    
private:
    double m_radius;
    unsigned m_sectors, m_stacks;
    Vector3 m_pos;

    void gen_sphere();

    std::vector<Vertex> m_vertices;
};
