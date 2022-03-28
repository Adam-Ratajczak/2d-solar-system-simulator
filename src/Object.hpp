#pragma once

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
    sf::Color m_color;
    unsigned m_tres;
    std::list<std::pair<Vector2, Vector2>> m_trail;
    double m_radius;
    Vector2 attraction(const Object&);

    double m_ap = 0, m_pe = std::numeric_limits<double>::max(), m_orbit;
    double m_ap_vel = 0, m_pe_vel = 0;

public:
    Object(double mass, double radius, Vector2 pos, Vector2 vel, sf::Color color, std::string name, unsigned tres);
    Vector2 m_pos;
    Vector2 m_vel;
    std::string m_name;
    bool m_focused = 0;
    double m_mass;

    void update();
    void draw(SimulationView const&);
    bool hover(SimulationView& view, Vector2 mouse_pos);
    void calculate_propieties();
    void add_object(double mass, double radius, double distance, Angle theta, double velocity, sf::Color color, std::string name, unsigned tres);
};

inline bool operator==(const Object& a, const Object& b)
{
    return &a == &b;
}
