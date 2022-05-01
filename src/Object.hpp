#pragma once

#include "History.hpp"
#include "SimulationView.hpp"
#include "Trail.hpp"
#include "glwrapper/Sphere.hpp"
#include "math/Vector3.hpp"
#include "pyssa/Object.hpp"
#include "pyssa/WrappedObject.hpp"
#include "util/SimulationClock.hpp"
#include "util/Units.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <limits>
#include <list>
#include <string>
#include <vector>

class Object : public PySSA::WrappedObject<Object> {
public:
    Object(double mass, double radius, Vector3 pos, Vector3 vel, sf::Color color, std::string name, unsigned period);

    static Object* create_for_python(PySSA::Object const& args, PySSA::Object const& kwargs);

    Object(Object const& other) = delete;
    Object& operator=(Object const& other) = delete;
    Object(Object&& other) = delete;
    Object& operator=(Object&& other) = delete;

    Vector3 render_position() const { return m_pos / AU; }

    std::string name() const { return m_name; }

    void setup_update_forces();
    void update_forces_against(Object&);
    void update(int speed);
    Trail& trail() { return m_trail; }

    // Draw the object in world's coordinates.
    void draw(SimulationView const&);

    // Draw the GUI layer of the object (without world transform)
    void draw_gui(SimulationView const&);

    void draw_closest_approaches(SimulationView const&);
    void draw_closest_approaches_gui(SimulationView const&);

    void calculate_propieties();
    std::unique_ptr<Object> create_object_relative_to(double mass, Distance radius, Distance apogee, Distance perigee, bool direction, Angle theta, Angle alpha, sf::Color color, std::string name, Angle rotation);
    void add_object_relative_to(double mass, Distance radius, Distance apogee, Distance perigee, bool direction, Angle theta, Angle alpha, sf::Color color, std::string name, Angle rotation = 0.0_rad);

    std::unique_ptr<Object> clone_for_forward_simulation() const;

    // Calculates velocity and direction so that the orbit passes through
    // the given point.
    void require_orbit_point(Vector3);

    double mass() const { return m_gravity_factor / G; }
    double gravity_factor() const { return m_gravity_factor; }

    Vector3 vel() const {return m_vel;}
    Vector3 pos() const {return m_pos;}

    sf::Color color() const{return m_color;}
    
    Util::SimulationClock::time_point creation_date() const { return m_creation_date; }

    static void setup_python_bindings(TypeSetup);
    static constexpr char const* PythonClassName = "Object";

    double radius() const { return m_radius; }
    void set_radius(double radius);

    Sphere& sphere() { return m_sphere; }

    Object* most_attracting_object() const { return m_most_attracting_object; }
    void delete_most_attracting_object();

    struct Info {
        double mass;
        double radius;
        double absolute_velocity;

        // Only if orbiting around more massive object.
        double distance_from_most_massive_object = 0; // in AU
        double apogee = 0;                            // in AU
        double apogee_velocity = 0;
        double perigee = 0; // in AU
        double perigee_velocity = 0;
        double orbit_period = 0;
        double orbit_eccencrity = 0;
    };

    Info get_info() const;

    void reset_history();

private:
    friend class World;
    friend std::ostream& operator<<(std::ostream& out, Object const&);

    void update_closest_approaches();

    // Draws label in at 3d position but not projected (in GUI layer).
    void draw_label(SimulationView const&, Vector3 position, std::string, sf::Color) const;

    PySSA::Object python_attraction(PySSA::Object const& args);

    PySSA::Object python_get_pos() const;
    bool python_set_pos(PySSA::Object const&);
    PySSA::Object python_get_vel() const;
    bool python_set_vel(PySSA::Object const&);
    PySSA::Object python_get_name() const;
    bool python_set_name(PySSA::Object const&);
    PySSA::Object python_get_color() const;
    bool python_set_color(PySSA::Object const&);
    PySSA::Object python_get_radius() const;
    bool python_set_radius(PySSA::Object const&);

    double m_density;
    Trail m_trail;
    History m_history;

    // NOTE: We only keep that for Python. It's not used anywhere.
    Vector3 attraction(const Object&);
    void recalculate_trails_with_offset();

    Vector3 m_pos;

    double m_ap = 0, m_pe = std::numeric_limits<double>::max();
    double m_ap_vel = 0, m_pe_vel = 0;
    World* m_world = nullptr;
    Vector3 m_attraction_factor;
    float m_prev_zoom;
    double m_orbit_len, eccencrity;
    double m_gravity_factor {};
    double m_radius {};

    bool m_is_forward_simulated = false;
    Sphere m_sphere;
    Util::SimulationClock::time_point m_creation_date;
    Vector3 m_vel;
    std::string m_name;
    sf::Color m_color;

    double m_max_attraction = 0;
    Object* m_old_most_attracting_object = nullptr;
    Object* m_most_attracting_object = nullptr;

    struct ClosestApproachEntry {
        Vector3 this_position;
        Vector3 other_object_position;
        double distance {};
    };
    std::map<Object*, ClosestApproachEntry> m_closest_approaches;
};
