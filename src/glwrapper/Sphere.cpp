#include <GL/gl.h>

#include <iostream>
#include <iomanip>
#include <cmath>
#include "Color.hpp"
#include "Sphere.hpp"


const int MIN_SECTOR_COUNT = 3;
const int MIN_STACK_COUNT  = 2;


Sphere::Sphere(Vector3 pos, double radius, int sectors, int stacks) : m_pos(pos), m_radius(radius), m_sectors(sectors), m_stacks(stacks){
    gen_sphere();
}

void Sphere::gen_sphere(){
    unsigned i = 0;
    double delta_theta = M_PI / m_stacks, delta_alpha = 2 * M_PI / m_sectors;
    for(double theta = -M_PI / 2; theta <= M_PI / 2; theta += delta_theta){
        for(double alpha = 0; alpha <= 2 * M_PI; alpha += delta_alpha){
                m_vertices.push_back({
                    Vector3(m_radius * std::cos(theta) * std::cos(alpha), 
                        m_radius * std::sin(theta), 
                        m_radius * std::cos(theta) * std::sin(alpha)) + m_pos,
                        Color()});
        }
        i++;
    }
}

void Sphere::set_colors(std::initializer_list<std::pair<Color, int>> list){
    for(const auto& color : list){
        for(unsigned i = 0; i < m_stacks; i++){
            double mul = 1.0 / std::fabs((float)i / (float)color.second);
            for(unsigned j = 0; j < m_sectors; j++){
                m_vertices[i * m_sectors + j].color += color.first * mul;
            }
        }
    }
}

void Sphere::draw(sf::RenderWindow &window) const{
    window.popGLStates();
    for(unsigned i = 0; i < m_stacks; i++){
        for(unsigned j = 0; j < 2 * m_sectors; j++){
        glBegin(GL_QUADS);
            m_vertices[i * m_sectors + j].color.glDraw();
            m_vertices[i * m_sectors + j].position.glDraw();
            m_vertices[i * m_sectors + j + 1].color.glDraw();
            m_vertices[i * m_sectors + j + 1].position.glDraw();
            m_vertices[(i + 1) * m_sectors + j + 1].color.glDraw();
            m_vertices[(i + 1) * m_sectors + j + 1].position.glDraw();
            m_vertices[(i + 1) * m_sectors + j].color.glDraw();
            m_vertices[(i + 1) * m_sectors + j].position.glDraw();
        glEnd();
        }
    }
    window.pushGLStates();
}
