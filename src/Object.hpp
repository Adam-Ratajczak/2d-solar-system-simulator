#pragma once

#include "Vector2.hpp"
#include "View.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <limits>
#include <list>

class Object{
protected:
    double m_density;
    Vector2 m_vel;
    sf::Color m_color;
    unsigned m_tres;
    std::list<Vector2> m_trail;
    double m_radius;
    Vector2 attraction(const Object&);

    double m_ap = 0, m_pe = std::numeric_limits<double>::max(), m_orbit;
    double m_ap_vel = 0, m_pe_vel = 0;

public:
    Object(double mass, double radius, Vector2 pos, Vector2 vel, sf::Color color, std::string name, unsigned tres);
    Vector2 m_pos;
    std::string m_name;
    bool m_focused = 0;
    double m_mass;
    
    void update(std::list<Object>& object_list);
    void draw(View&);
    bool hover(View& view, Vector2 mouse_pos);
    void calculate_propieties();
};

inline bool operator==(const Object& a, const Object& b){return &a == &b;}
