#pragma once

#include "Trail.hpp"
#include "Vector3.hpp"
#include "gui/SimulationView.hpp"
#include "gui/Units.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <limits>
#include <list>
#include <string>

class Object {
protected:
    double m_density;
    Trail m_trail;
    Vector3 attraction(const Object&);

    // FIXME: What is m_ap, m_pe, m_ap_vel, m_pe_vel?
    double m_ap = 0, m_pe = std::numeric_limits<double>::max();
    double m_ap_vel = 0, m_pe_vel = 0;
    World& m_world;
    void m_draw_trail();
    Vector3 m_prev_offset;
    Vector3 m_attraction_factor;
    float m_prev_zoom;
    double m_orbit_len, eccencrity;
    double m_gravity_factor {};

    bool m_is_forward_simulated = false;

public:
    // FIXME: Too much arguments!!!
    // FIXME: what is "tres"?
    Object(World& world, double mass, double radius, Vector3 pos, Vector3 vel, sf::Color color, std::string name, unsigned period);

    Object(Object const& other) = delete;
    Object& operator=(Object const& other) = delete;
    Object(Object&& other) = delete;
    Object& operator=(Object&& other) = delete;

    Vector3 m_pos;
    Vector3 m_vel;
    std::string m_name;
    bool m_focused = 0;
    sf::Color m_color;
    double m_radius;

    void update_forces(bool reverse);
    void update();
    Trail& trail() { return m_trail; }

    // Draw the object in world's coordinates.
    void draw(SimulationView const&);

    // Draw the GUI layer of the object (without world transform)
    void draw_gui(SimulationView const&);

    bool hover(SimulationView& view, Vector3 mouse_pos);
    void calculate_propieties();
    std::unique_ptr<Object> create_object_relative_to(double mass, Distance radius, Distance apogee, Distance perigee, bool direction, Angle theta, Angle alpha, sf::Color color, std::string name, Angle rotation);
    void add_object_relative_to(double mass, Distance radius, Distance apogee, Distance perigee, bool direction, Angle theta, Angle alpha, sf::Color color, std::string name, Angle rotation = 0.0_rad);

    std::unique_ptr<Object> clone_for_forward_simulation(World& new_world) const;

    double mass() const { return m_gravity_factor / G; }
    double gravity_factor() const { return m_gravity_factor; }
};
