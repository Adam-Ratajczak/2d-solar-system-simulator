#include "Object.hpp"
#include "SimulationView.hpp"
#include "World.hpp"
#include "glwrapper/Helpers.hpp"
#include "glwrapper/Sphere.hpp"
#include "gui/Application.hpp"
#include "gui/Units.hpp"
#include "math/Transform.hpp"
#include "math/Vector3.hpp"
#include "pyssa/Object.hpp"
#include "pyssa/TupleParser.hpp"

#include <GL/gl.h>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstring>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

Object::Object(World& world, double mass, double radius, Vector3 pos, Vector3 vel, sf::Color color, std::string name, unsigned period)
    : m_world(world)
    , m_trail(period * 2, color)
    // FIXME: Share the sphere as it is identical for all objects and
    //        takes most of the object's used memory.
    , m_sphere(radius / AU, 36, 18)
    , m_history(period, { pos, vel })
    , m_gravity_factor(mass * G)
    , m_radius(radius)
    , m_pos(pos)
    , m_vel(vel)
    , m_color(color)
    , m_name(name)
    , m_orbit_len(period)
    , m_creation_date(world.date().get_int()) {
    m_trail.push_back(m_pos);
    m_sphere.set_color(m_color);
}

Vector3 Object::attraction(const Object& other) {
    Vector3 dist = this->m_pos - other.m_pos;
    double force = other.m_gravity_factor / dist.magnitude_squared();
    Vector3 normalized_dist = dist.normalized();
    return normalized_dist * force;
}

void Object::setup_update_forces() {
    m_attraction_factor = Vector3();
    m_max_attraction = 0;
}

void Object::update_forces_against(Object& object) {
    // TODO: Collisions
    Vector3 dist = this->m_pos - object.m_pos;
    // denominator: R^2*normalized(dist)
    double denominator = dist.magnitude_squared();
    denominator *= std::sqrt(denominator);
    auto attraction_base = dist / denominator;

    // TODO: Do it properly
    auto this_attraction = attraction_base * object.m_gravity_factor;
    m_attraction_factor -= this_attraction;
    auto this_attraction_mag = this_attraction.magnitude_squared() / object.m_gravity_factor;

    if (this_attraction_mag > m_max_attraction && object.m_gravity_factor > m_gravity_factor) {
        m_max_attraction = this_attraction_mag;
        m_most_attracting_object = &object;
    }

    auto other_attraction = attraction_base * m_gravity_factor;
    object.m_attraction_factor += other_attraction;

    auto other_attraction_mag = other_attraction.magnitude_squared() / m_gravity_factor;
    if (other_attraction_mag > object.m_max_attraction && object.m_gravity_factor < m_gravity_factor) {
        object.m_max_attraction = other_attraction_mag;
        object.m_most_attracting_object = this;
    }
}

void Object::update(int speed) {
    if (m_history.on_edge()) {
        m_vel += m_attraction_factor * m_world.simulation_seconds_per_tick();

        if (speed > 0)
            m_pos += m_vel * m_world.simulation_seconds_per_tick();
        else if (speed < 0)
            m_pos -= m_vel * m_world.simulation_seconds_per_tick();
    }
    else {
        auto entry = m_history.get_entry();
        m_pos = entry.pos;
        m_vel = entry.vel;
    }

    if (speed > 0)
        m_history.push_back({ m_pos, m_vel });
    else if (speed < 0)
        m_history.push_front();

    m_trail.push_back(m_pos);

    if (!m_most_attracting_object)
        return;

    double distance_from_object = get_distance(this->m_pos, m_most_attracting_object->m_pos);
    if (m_ap < distance_from_object) {
        m_ap = distance_from_object;
        m_ap_vel = m_vel.magnitude();
    }

    if (m_pe > distance_from_object) {
        m_pe = distance_from_object;
        m_pe_vel = m_vel.magnitude();
    }
    // std::cout << m_name << ": " << m_trail.size() << "\n";
}

void Object::draw(SimulationView const& view) {

    auto scaled_pos = render_position();
    auto& target = view.window();

    m_sphere.set_position(scaled_pos);
    m_sphere.draw();

    if (view.show_trails())
        m_trail.draw();
}

Object::Info Object::get_info() const {
    Info info {
        .mass = mass(),
        .radius = m_radius,
        .absolute_velocity = m_vel.magnitude()
    };

    if (m_most_attracting_object) {
        info.distance_from_most_massive_object = get_distance(this->m_pos, m_most_attracting_object->m_pos) / AU;
        info.apogee = m_ap / AU;
        info.apogee_velocity = m_ap_vel;
        info.perigee = m_pe / AU;
        info.perigee_velocity = m_pe_vel;
        info.orbit_period = m_orbit_len / 365.25;
        info.orbit_eccencrity = eccencrity;
    }

    return info;
}

void Object::reset_history() {
    m_history.reset();
    m_trail.reset();

    auto entry = m_history.first_entry();
    m_pos = entry.pos;
    m_vel = entry.vel;
}

void Object::draw_gui(SimulationView const& view) {
    auto position = view.world_to_screen(render_position());

    // Don't draw labels of planets outside of clipping box
    if (position.z > 1 || position.z < -1)
        return;

    if (!view.show_labels())
        return;
    sf::Text text(m_name, GUI::Application::the().font, 15);
    text.setPosition({ static_cast<float>(position.x), static_cast<float>(position.y) });
    if (m_is_forward_simulated)
        text.setFillColor(sf::Color(128, 128, 128));
    view.window().draw(text);
}

std::unique_ptr<Object> Object::create_object_relative_to(double mass, Distance radius, Distance apogee, Distance perigee, bool direction, Angle theta, Angle alpha, sf::Color color, std::string name, Angle rotation) {
    // formulae used from site: https://www.scirp.org/html/6-9701522_18001.htm
    // std::cout << m_gravity_factor << "\n";
    double GM = m_gravity_factor;
    double a = (apogee.value() + perigee.value()) / 2;
    double b = std::sqrt(apogee.value() * perigee.value());

    double T = 2 * M_PI * std::sqrt((a * a * a) / GM);
    // T = T * std::sin(theta.rad()) + T * std::cos(alpha.rad());
    Vector3 pos(std::cos(theta.rad()) * std::cos(alpha.rad()) * a, std::sin(theta.rad()) * std::cos(alpha.rad()) * a, std::sin(alpha.rad()) * a);
    pos = pos.rotate_vector(rotation.rad());
    pos += this->m_pos;

    auto result = std::make_unique<Object>(m_world, mass, radius.value(), pos, Vector3(0, 0), color, name, T / (3600 * 24));
    result->m_ap = apogee.value();
    result->m_pe = perigee.value();

    result->eccencrity = std::sqrt(1 - (b * b) / (a * a));
    // std::cout << result.eccencrity << "\n";

    double velocity_constant = (2 * GM) / (apogee.value() + perigee.value());

    result->m_ap_vel = std::sqrt(2 * GM / apogee.value() - velocity_constant);
    result->m_pe_vel = std::sqrt(2 * GM / perigee.value() - velocity_constant);
    double velocity = std::sqrt(2 * GM / get_distance(pos, this->m_pos) - velocity_constant);

    Vector3 vel(std::cos(theta.rad() + M_PI / 2) * velocity, std::sin(theta.rad() + M_PI / 2) * velocity);

    if (direction)
        vel = -vel;

    vel += this->m_vel;
    result->m_vel = vel;

    return result;
}

void Object::add_object_relative_to(double mass, Distance radius, Distance apogee, Distance perigee, bool direction, Angle theta, Angle alpha, sf::Color color, std::string name, Angle rotation) {
    m_world.add_object(create_object_relative_to(mass, radius, apogee, perigee, direction, theta, alpha, color, name, rotation));
}

std::unique_ptr<Object> Object::clone_for_forward_simulation(World& new_world) const {
    auto brightened_color = [](sf::Color const& color) {
        return sf::Color {
            (uint8_t)std::min(255, color.r + 60),
            (uint8_t)std::min(255, color.g + 60),
            (uint8_t)std::min(255, color.b + 60),
            255
        };
    };
    auto object = std::make_unique<Object>(new_world, mass(), m_radius, m_pos, m_vel, brightened_color(m_color), m_name, 1000);
    object->m_is_forward_simulated = true;
    return object;
}

void Object::set_radius(double radius) {
    m_radius = radius;
}

std::ostream& operator<<(std::ostream& out, Object const& object) {
    return out << "Object(" << object.m_name << ") @ " << object.m_pos;
}

void Object::setup_python_bindings(TypeSetup setup) {
    setup.add_method<&Object::python_attraction>("attraction");
    setup.add_attribute<&Object::python_get_pos, nullptr>("pos");
    setup.add_attribute<&Object::python_get_vel, nullptr>("vel");
    setup.add_attribute<&Object::python_get_name, nullptr>("name");
    setup.add_attribute<&Object::python_get_focused, nullptr>("focused");
    setup.add_attribute<&Object::python_get_color, nullptr>("color");
    setup.add_attribute<&Object::python_get_radius, nullptr>("radius");
}

Object* Object::create_for_python(PySSA::Object const& args, PySSA::Object const& kwargs) {
    PyObject* world = nullptr;
    double mass = 0;
    double radius = 0;
    Vector3 pos;
    Vector3 vel;
    sf::Color color;
    char const* name = "Planet";
    unsigned period = 1;

    static char const* keywords[] = {
        "world",
        "mass",
        "radius",
        "pos",
        "vel",
        "color",
        "name",
        "period",
        nullptr
    };

    if (!PyArg_ParseTupleAndKeywords(args.python_object(), kwargs.python_object(), "O!|$dd(ddd)(ddd)(bbb)su", (char**)keywords,
            &World::type_object(),
            &world,
            &mass,
            &radius,
            &pos.x, &pos.y, &pos.z,
            &vel.x, &vel.y, &vel.z,
            &color.r, &color.g, &color.b,
            &name,
            &period))
        return {};

    return new Object(*World::get(PySSA::Object::share(world)), mass, radius, pos, vel, color, name, period);
}

PySSA::Object Object::python_attraction(PySSA::Object const& args) {
    Object* other {};
    if (!PySSA::parse_arguments(args, "O!", PySSA::Arg::CheckedType<Object> { other }))
        return {};
    return PySSA::Object::create(attraction(*other));
}

PySSA::Object Object::python_get_pos() const {
    return PySSA::Object::create(m_pos);
}

PySSA::Object Object::python_get_vel() const {
    return PySSA::Object::create(m_vel);
}

PySSA::Object Object::python_get_name() const {
    return PySSA::Object::create(m_name);
}

PySSA::Object Object::python_get_focused() const {
    return PySSA::Object::create(this == m_world.m_simulation_view->focused_object());
}

PySSA::Object Object::python_get_color() const {
    return PySSA::Object::create(m_color);
}

PySSA::Object Object::python_get_radius() const {
    return PySSA::Object::create(m_radius);
}
