#pragma once

#include "History.hpp"
#include "SimulationView.hpp"
#include "Trail.hpp"
#include "glwrapper/Sphere.hpp"
#include "gui/Units.hpp"
#include "math/Vector3.hpp"
#include "pyssa/Object.hpp"
#include "pyssa/WrappedObject.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <limits>
#include <list>
#include <string>
#include <vector>

class Object : public PySSA::WrappedObject<Object> {
protected:
    double m_density;
    Trail m_trail;
    History m_history;
    Vector3 attraction(const Object&);

    Vector3 m_pos;

    double m_ap = 0, m_pe = std::numeric_limits<double>::max();
    double m_ap_vel = 0, m_pe_vel = 0;
    World& m_world;
    Vector3 m_attraction_factor;
    float m_prev_zoom;
    double m_orbit_len, eccencrity;
    double m_gravity_factor {};
    double m_radius {};

    bool m_is_forward_simulated = false;
    Sphere m_sphere;
    int m_creation_date;
    Vector3 m_vel;
    std::string m_name;
    sf::Color m_color;

public:
    Object(World& world, double mass, double radius, Vector3 pos, Vector3 vel, sf::Color color, std::string name, unsigned period);

    static Object* create_for_python(PySSA::Object const& args, PySSA::Object const& kwargs);

    Object(Object const& other) = delete;
    Object& operator=(Object const& other) = delete;
    Object(Object&& other) = delete;
    Object& operator=(Object&& other) = delete;

    Vector3 render_position() const { return m_pos / AU; }

    std::string name() const { return m_name; }

    void update_forces(bool reverse);
    void update(int speed);
    Trail& trail() { return m_trail; }

    // Draw the object in world's coordinates.
    void draw(SimulationView const&);

    // Draw the GUI layer of the object (without world transform)
    void draw_gui(SimulationView const&);

    bool hover(SimulationView& view, Vector3 mouse_pos) const;
    void calculate_propieties();
    std::unique_ptr<Object> create_object_relative_to(double mass, Distance radius, Distance apogee, Distance perigee, bool direction, Angle theta, Angle alpha, sf::Color color, std::string name, Angle rotation);
    void add_object_relative_to(double mass, Distance radius, Distance apogee, Distance perigee, bool direction, Angle theta, Angle alpha, sf::Color color, std::string name, Angle rotation = 0.0_rad);

    std::unique_ptr<Object> clone_for_forward_simulation(World& new_world) const;

    double mass() const { return m_gravity_factor / G; }
    double gravity_factor() const { return m_gravity_factor; }
    int creation_date() const { return m_creation_date; }

    static void setup_python_bindings(TypeSetup);
    static constexpr char const* PythonClassName = "Object";

    double radius() const { return m_radius; }
    void set_radius(double radius);

    Sphere& sphere() { return m_sphere; }

    std::vector<std::string> get_info() const;

    void reset_history();

private:
    friend std::ostream& operator<<(std::ostream& out, Object const&);

    PySSA::Object python_attraction(PySSA::Object const& args);

    PySSA::Object python_get_pos() const;
    PySSA::Object python_get_vel() const;
    PySSA::Object python_get_name() const;
    PySSA::Object python_get_focused() const;
    PySSA::Object python_get_color() const;
    PySSA::Object python_get_radius() const;
};
