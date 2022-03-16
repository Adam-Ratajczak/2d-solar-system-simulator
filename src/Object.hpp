#pragma once

#include "Vector2.hpp"
#include "View.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <list>

class Object{
protected:
    double m_mass, m_density;
    Vector2 m_vel;
    sf::Color m_color;
    unsigned m_tres;
    std::list<Vector2> m_trail;
    double m_radius;
    Vector2 attraction(const Object&);

public:
    Vector2 m_pos;
    std::string m_name;
    
    void draw(View&);
    bool hover(View& view, Vector2 mouse_pos);
};