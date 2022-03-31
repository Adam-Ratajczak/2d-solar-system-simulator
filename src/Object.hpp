#pragma once

#include "Trail.hpp"
#include "Vector2.hpp"
#include "gui/SimulationView.hpp"
#include "gui/Units.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <limits>
#include <list>
#include <string>

class Object
{
protected:
    double m_density;
    // FIXME: What is m_tres?
    unsigned m_tres;
    Trail m_trail;
    Vector2 attraction(const Object&);

    // FIXME: What is m_ap, m_pe, m_ap_vel, m_pe_vel?
    double m_ap = 0, m_pe = std::numeric_limits<double>::max(), m_orbit;
    double m_ap_vel = 0, m_pe_vel = 0;
    World& m_world;
    void m_draw_trail();
    Vector2 m_prev_offset;
    float m_prev_zoom;
    double m_orbit_len, eccencrity;

public:
    // FIXME: Too much arguments!!!
    // FIXME: what is "tres"?
    Object(World& world, double mass, double radius, Vector2 pos, Vector2 vel, sf::Color color, std::string name, unsigned tres);
    Vector2 m_pos;
    Vector2 m_vel;
    std::string m_name;
    bool m_focused = 0;
    double m_mass;
    sf::Color m_color;
    double m_radius;

    void update();
    Trail& trail(){return m_trail;}
    void draw(SimulationView const&);
    bool hover(SimulationView& view, Vector2 mouse_pos);
    void calculate_propieties();
    void add_object_relative_to(double mass, double radius, double apogee, double perigee, bool direction, Angle theta, sf::Color color, std::string name);
};

inline bool operator==(const Object& a, const Object& b)
{
    return a.m_name == b.m_name && a.m_mass == b.m_mass && a.m_radius == b.m_radius && a.m_color == b.m_color && a.m_pos == b.m_pos && a.m_vel == b.m_vel;
}

inline bool operator!=(const Object& a, const Object& b)
{
    return !(a == b);
}
