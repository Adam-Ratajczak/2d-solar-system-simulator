#pragma once

#include <GL/glew.h>

#include "History.hpp"
#include "SimulationView.hpp"
#include "Trail.hpp"
#include "glwrapper/Sphere.hpp"
#include "pyssa/Object.hpp"
#include "pyssa/WrappedObject.hpp"

#include <EssaUtil/SimulationClock.hpp>
#include <EssaUtil/Units.hpp>
#include <EssaUtil/Vector.hpp>
#include <limits>
#include <list>
#include <memory>
#include <string>
#include <vector>

class Object : public PySSA::WrappedObject<Object> {
public:
    Object(double mass, double radius, Util::DeprecatedVector3d pos, Util::DeprecatedVector3d vel, Util::Color color, Util::UString name, unsigned period);

#ifdef ENABLE_PYSSA
    static Object* create_for_python(PySSA::Object const& args, PySSA::Object const& kwargs);
#endif

    Object(Object const& other) = delete;
    Object& operator=(Object const& other) = delete;
    Object(Object&& other) = delete;
    Object& operator=(Object&& other) = delete;

    Util::DeprecatedVector3d render_position() const { return m_pos / Util::Constants::AU; }

    Util::UString name() const { return m_name; }

    // Called before each force calculation pass (currently Leapfrog step)
    void clear_forces();

    // Called before anything physical is done on the Object.
    void before_update();

    void update_forces_against(Object&);
    void update(int speed);

    Trail& trail() { return m_trail; }
    static Sphere& sphere();

    // Draw the object in world's coordinates.
    void draw(Gfx::Painter&, SimulationView const&);

    // Draw the GUI layer of the object (without world transform)
    void draw_gui(Gfx::Painter&, SimulationView const&);

    void draw_closest_approaches(Gfx::Painter& window, SimulationView const&);
    void draw_closest_approaches_gui(Gfx::Painter& window, SimulationView const&);

    void set_display_lagrange_points(bool display) { m_display_lagrange_points = display; }

    void calculate_propieties();
    std::unique_ptr<Object> create_object_relative_to_ap_pe(double mass, Distance radius, Distance apoapsis, Distance periapsis, bool direction, Util::Angle theta, Util::Angle alpha, Util::Color color, Util::UString name, Util::Angle rotation);
    std::unique_ptr<Object> create_object_relative_to_maj_ecc(double mass, Distance radius, Distance semi_major, double ecc, bool direction, Util::Angle theta, Util::Angle alpha, Util::Color color, Util::UString name, Util::Angle rotation);
    void add_object_relative_to(double mass, Distance radius, Distance apoapsis, Distance periapsis, bool direction, Util::Angle theta, Util::Angle alpha, Util::Color color, Util::UString name, Util::Angle rotation = 0.0_rad);

    std::unique_ptr<Object> clone_for_forward_simulation() const;

    // Calculates velocity and direction so that the orbit passes through
    // the given point.
    void require_orbit_point(Util::DeprecatedVector3d);

    double mass() const { return m_gravity_factor / Util::Constants::Gravity; }
    double gravity_factor() const { return m_gravity_factor; }

    Util::DeprecatedVector3d pos() const { return m_pos; }
    void set_pos(const Util::DeprecatedVector3d& pos) { m_pos = pos; }

    Util::DeprecatedVector3d vel() const { return m_vel; }
    void set_vel(const Util::DeprecatedVector3d& vel) { m_vel = vel; }

    Util::DeprecatedVector3d acc() const { return m_attraction_factor; }

    Util::Color color() const { return m_color; }

    Util::SimulationClock::time_point creation_date() const { return m_creation_date; }

    Util::SimulationClock::time_point deletion_date() const { return m_deletion_date; }
    bool deleted() const;
    void delete_object();

#ifdef ENABLE_PYSSA
    static void setup_python_bindings(TypeSetup);
    static constexpr char const* PythonClassName = "Object";
#endif

    double radius() const { return m_radius; }
    void set_radius(double radius);

    Object* most_attracting_object() const { return m_most_attracting_object; }
    void delete_most_attracting_object();

    struct Info {
        double mass;
        double radius;
        double absolute_velocity;

        // Only if orbiting around more massive object.
        double distance_from_most_massive_object = 0; // in AU
        double apoapsis = 0;                            // in AU
        double apoapsis_velocity = 0;
        double periapsis = 0; // in AU
        double periapsis_velocity = 0;
        double orbit_period = 0;
        double orbit_eccentrity = 0;
    };

    Info get_info() const;

    void reset_history();

private:
    friend class World;
    friend std::ostream& operator<<(std::ostream& out, Object const&);

    void update_closest_approaches();

    // Called after everything physical is done on the Object.
    // Used for trails / apoapsis & periapsis / everything that doesn't
    // influence physical behavior
    void nonphysical_update();

    // Draws label in at 3d position but not projected (in GUI layer).
    void draw_label(Gfx::Painter&, SimulationView const&, Util::DeprecatedVector3d position, Util::UString string, Util::Color) const;

#ifdef ENABLE_PYSSA
    PySSA::Object python_attraction(PySSA::Object const& args, PySSA::Object const& kwargs);

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
    PySSA::Object python_get_mass() const;
    bool python_set_mass(PySSA::Object const&);
#endif

    double m_density;
    Trail m_trail;
    History m_history;

    // NOTE: We only keep that for Python. It's not used anywhere.
    Util::DeprecatedVector3d attraction(const Object&);
    void recalculate_trails_with_offset();

    Util::DeprecatedVector3d m_pos;
    Util::DeprecatedVector3d m_vel;
    Util::DeprecatedVector3d m_attraction_factor;

    bool m_deleted = false;
    bool m_is_forward_simulated = false;

    World* m_world = nullptr;

    double m_ap = 0, m_pe = std::numeric_limits<double>::max();
    double m_ap_vel = 0, m_pe_vel = 0;
    float m_prev_zoom;
    double m_orbit_len, eccentrity;
    double m_gravity_factor {};
    double m_radius {};

    Util::SimulationClock::time_point m_creation_date, m_deletion_date;
    Util::UString m_name;
    Util::Color m_color;

    double m_max_attraction = 0;
    Object* m_old_most_attracting_object = nullptr;
    Object* m_most_attracting_object = nullptr;

    struct ClosestApproachEntry {
        Util::DeprecatedVector3d this_position;
        Util::DeprecatedVector3d other_object_position;
        double distance {};
    };
    std::map<Object*, ClosestApproachEntry> m_closest_approaches;

    bool m_display_lagrange_points = false;
};
