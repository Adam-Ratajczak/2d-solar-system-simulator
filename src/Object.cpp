#include "Object.hpp"
#include "Transform.hpp"
#include "Vector3.hpp"
#include "World.hpp"
#include "glwrapper/Helpers.hpp"
#include "glwrapper/Sphere.hpp"
#include "gui/GUI.hpp"
#include "gui/SimulationView.hpp"
#include "gui/Units.hpp"
#include "modsupport.h"
#include "pyssa/Object.hpp"
#include <GL/gl.h>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cmath>
#include <cstring>
#include <iostream>
#include <string>
#include <utility>

Object::Object(World& world, double mass, double radius, Vector3 pos, Vector3 vel, sf::Color color, std::string name, unsigned period)
    : m_world(world)
    // FIXME: Very hacky fix for trail.
    , m_trail(period * 2)
    , m_sphere(radius / AU, 36, 18) {
    m_gravity_factor = mass * G;
    m_radius = radius;
    m_pos = pos;
    m_vel = vel;
    m_color = color;
    m_name = name;
    m_orbit_len = period;

    m_display_trail.resize(period * 2);

    m_sphere.set_color(m_color);
    m_trail.push_back(m_pos);
}

Vector3 Object::attraction(const Object& other) {
    Vector3 dist = this->m_pos - other.m_pos;
    double force = other.m_gravity_factor / (dist.x * dist.x + dist.y * dist.y);
    Vector3 normalized_dist = dist.normalized();
    return normalized_dist * force;
}

bool Object::hover(SimulationView& view, Vector3 mouse_pos) const {
    auto position = view.world_to_screen(m_pos / AU);
    auto dist = get_distance(position, mouse_pos);

    // std::cout << dist << "\n";

    // return get_distance(position, mouse_pos) < 20;
    return false;
}

void Object::update_forces(bool reverse) {
    m_attraction_factor = Vector3();
    m_world.for_each_object([&](Object& object) {
        // TODO: Collisions
        if (this != &object)
            m_attraction_factor -= attraction(object);
    });
}

void Object::update() {
    m_vel += m_attraction_factor * m_world.simulation_seconds_per_tick();
    m_pos += m_vel * m_world.simulation_seconds_per_tick();

    m_display_trail[m_display_trail_append_offset] = Vertex { .position = m_pos / AU, .color = m_color };
    m_display_trail_append_offset++;
    if (m_display_trail_length < m_display_trail.size())
        m_display_trail_length++;
    if (m_display_trail_append_offset >= m_display_trail.size())
        m_display_trail_append_offset = 0;

    // if(m_pos != m_trail.back())
    m_trail.push_back(m_pos);

    if (m_trail.size() > m_orbit_len)
        m_trail.pop_front();
    m_trail.update_trail(*m_world.m_simulation_view, m_color);

    auto most_massive_object = m_world.most_massive_object();
    if (most_massive_object == this)
        return;

    double distance_from_object = get_distance(this->m_pos, most_massive_object->m_pos);
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

    // FIXME: Hardcoded multiplier
    auto scaled_pos = m_pos / AU;
    auto& target = view.window();

    m_sphere.set_position(scaled_pos);
    m_sphere.draw();

    GL::draw_vertices(GL_LINE_STRIP, { m_display_trail.data(), m_display_trail_append_offset });
    GL::draw_vertices(GL_LINE_STRIP, { m_display_trail.data() + m_display_trail_append_offset, m_display_trail_length - m_display_trail_append_offset });

    // FIXME: This thing should be in a Widget.
    if (this->m_focused) {
        unsigned exponent = std::log10(this->mass());
        sf::Text mass("Mass: " + std::to_string(this->mass() / std::pow(10, exponent)) + " * 10 ^ " + std::to_string(exponent) + " kg", GUI::font, 15);
        mass.setFillColor(sf::Color::White);
        auto bounds = mass.getLocalBounds();
        mass.setPosition(sf::Vector2f(target.getSize().x - bounds.width - 10, 10 + 25 * 0));
        target.draw(mass);

        sf::Text radius("Radius: " + std::to_string((int)m_radius / 1000) + " km", GUI::font, 15);
        radius.setFillColor(sf::Color::White);
        bounds = radius.getLocalBounds();
        radius.setPosition(sf::Vector2f(target.getSize().x - bounds.width - 10, 10 + 25 * 1));
        target.draw(radius);

        sf::Text vel("Velocity: " + std::to_string((int)m_vel.magnitude()) + " m / s", GUI::font, 15);
        vel.setFillColor(sf::Color::White);
        bounds = vel.getLocalBounds();
        vel.setPosition(sf::Vector2f(target.getSize().x - bounds.width - 10, 10 + 25 * 2));
        target.draw(vel);

        auto most_massive_object = m_world.most_massive_object();

        if (this == most_massive_object)
            return;

        double distance_from_object = get_distance(this->m_pos, most_massive_object->m_pos);
        sf::Text dist("Distance from the " + most_massive_object->m_name + ": " + std::to_string(distance_from_object / AU) + " AU", GUI::font, 15);
        dist.setFillColor(sf::Color::White);
        bounds = dist.getLocalBounds();
        dist.setPosition(sf::Vector2f(target.getSize().x - bounds.width - 10, 10 + 25 * 3));
        target.draw(dist);

        sf::Text ap("Object apogee: " + std::to_string(m_ap / AU) + " AU", GUI::font, 15);
        ap.setFillColor(sf::Color::White);
        bounds = ap.getLocalBounds();
        ap.setPosition(sf::Vector2f(target.getSize().x - bounds.width - 10, 10 + 25 * 4));
        target.draw(ap);

        sf::Text ap_vel("Velocity at apogee: " + std::to_string((int)m_ap_vel) + " m / s", GUI::font, 15);
        ap_vel.setFillColor(sf::Color::White);
        bounds = ap_vel.getLocalBounds();
        ap_vel.setPosition(sf::Vector2f(target.getSize().x - bounds.width - 10, 10 + 25 * 5));
        target.draw(ap_vel);

        sf::Text pe("Object perigee: " + std::to_string(m_pe / AU) + " AU", GUI::font, 15);
        pe.setFillColor(sf::Color::White);
        bounds = pe.getLocalBounds();
        pe.setPosition(sf::Vector2f(target.getSize().x - bounds.width - 10, 10 + 25 * 6));
        target.draw(pe);

        sf::Text pe_vel("Velocity at perigee: " + std::to_string((int)m_pe_vel) + " m / s", GUI::font, 15);
        pe_vel.setFillColor(sf::Color::White);
        bounds = pe_vel.getLocalBounds();
        pe_vel.setPosition(sf::Vector2f(target.getSize().x - bounds.width - 10, 10 + 25 * 7));
        target.draw(pe_vel);

        sf::Text orbital_period("Orbital period: " + std::to_string(m_orbit_len / 365.25) + " years", GUI::font, 15);
        orbital_period.setFillColor(sf::Color::White);
        bounds = orbital_period.getLocalBounds();
        orbital_period.setPosition(sf::Vector2f(target.getSize().x - bounds.width - 10, 10 + 25 * 8));
        target.draw(orbital_period);

        sf::Text orbit_eccencrity("Eccencrity: " + std::to_string(eccencrity), GUI::font, 15);
        orbital_period.setFillColor(sf::Color::White);
        bounds = orbital_period.getLocalBounds();
        orbital_period.setPosition(sf::Vector2f(target.getSize().x - bounds.width - 10, 10 + 25 * 9));
        target.draw(orbit_eccencrity);
    }
}

void Object::draw_gui(SimulationView const& view) {
    // FIXME: Hardcoded multiplier
    auto position = view.world_to_screen(m_pos / AU);

    sf::Text text(m_name, GUI::font, 15);
    text.setPosition({ static_cast<float>(position.x), static_cast<float>(position.y) });
    // text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);

    if(view.show_labels())
        view.window().draw(text);
}

std::unique_ptr<Object> Object::create_object_relative_to(double mass, Distance radius, Distance apogee, Distance perigee, bool direction, Angle theta, Angle alpha, sf::Color color, std::string name, Angle rotation) {
    // formulae used from site: https://www.scirp.org/html/6-9701522_18001.htm
    double GM = m_gravity_factor;
    double a = (apogee.value() + perigee.value()) / 2;
    double b = std::sqrt(apogee.value() * perigee.value());

    double T = 2 * M_PI * std::sqrt((a * a * a) / GM);
    Vector3 pos(std::cos(theta.rad()) * std::cos(alpha.rad()) * a, std::sin(theta.rad()) * std::cos(alpha.rad()) * b, std::sin(alpha.rad()) * a);
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
    // TODO: C++ API for that
    PyObject* other_arg = nullptr;
    if (PyArg_ParseTuple(args.python_object(), "O!", &type_object(), &other_arg) < 0)
        return {};
    // FIXME: This could go without incrementing refcount
    auto other = get(PySSA::Object::share(other_arg));

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
    return PySSA::Object::create(m_focused);
}

PySSA::Object Object::python_get_color() const {
    return PySSA::Object::create(m_color);
}

PySSA::Object Object::python_get_radius() const {
    return PySSA::Object::create(m_radius);
}
