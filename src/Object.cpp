#include "Object.hpp"
#include "Transform.hpp"
#include "Vector3.hpp"
#include "World.hpp"
#include "gui/GUI.hpp"
#include "gui/Units.hpp"
#include <GL/gl.h>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Text.hpp>
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
    , m_trail(period * 2) {
    m_gravity_factor = mass * G;
    m_radius = radius * 100;
    m_pos = pos;
    m_vel = vel;
    m_color = color;
    m_name = name;
    m_orbit_len = period;

    m_trail.push_back(m_pos);
}

Vector3 Object::attraction(const Object& other) {
    Vector3 dist = this->m_pos - other.m_pos;
    double force = other.m_gravity_factor / (dist.x * dist.x + dist.y * dist.y);
    Vector3 normalized_dist = dist.normalized();
    return normalized_dist * force;
}

bool Object::hover(SimulationView& view, Vector3 mouse_pos) {
    double dst = mouse_pos.distance_to(m_pos);
    // if(m_name == "Mercury")
    //     std::cout << dst << " <? " << 20 / view.scale() << " s=" << view.scale() << std::endl;
    return dst < 20 / view.scale();
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

    //std::cout << "CORRECT according to opengl: " << m_name << " " << scaled_pos << " -> " << Transform::project(scaled_pos) << std::endl;

    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(m_color.r / 255.f, m_color.g / 255.f, m_color.b / 255.f);
    glVertex3f(scaled_pos.x - m_radius / AU, scaled_pos.y - m_radius / AU, scaled_pos.z);
    glVertex3f(scaled_pos.x + m_radius / AU, scaled_pos.y - m_radius / AU, scaled_pos.z);
    glVertex3f(scaled_pos.x - m_radius / AU, scaled_pos.y + m_radius / AU, scaled_pos.z);
    glVertex3f(scaled_pos.x + m_radius / AU, scaled_pos.y + m_radius / AU, scaled_pos.z);
    glEnd();

    // FIXME: This thing should be in a Widget.
    // TODO: Bring back that.
    // if (this->m_focused) {
    //     unsigned exponent = std::log10(this->mass());
    //     sf::Text mass("Mass: " + std::to_string(this->mass() / std::pow(10, exponent)) + " * 10 ^ " + std::to_string(exponent) + " kg", GUI::font, 15);
    //     mass.setFillColor(sf::Color::White);
    //     auto bounds = mass.getLocalBounds();
    //     mass.setPosition(sf::Vector2f(target.getSize().x - bounds.width - 10, 10 + 25 * 0));
    //     target.draw(mass);

    //     sf::Text radius("Radius: " + std::to_string((int)m_radius / 1000) + " km", GUI::font, 15);
    //     radius.setFillColor(sf::Color::White);
    //     bounds = radius.getLocalBounds();
    //     radius.setPosition(sf::Vector2f(target.getSize().x - bounds.width - 10, 10 + 25 * 1));
    //     target.draw(radius);

    //     sf::Text vel("Velocity: " + std::to_string((int)m_vel.magnitude()) + " m / s", GUI::font, 15);
    //     vel.setFillColor(sf::Color::White);
    //     bounds = vel.getLocalBounds();
    //     vel.setPosition(sf::Vector2f(target.getSize().x - bounds.width - 10, 10 + 25 * 2));
    //     target.draw(vel);

    //     auto most_massive_object = m_world.most_massive_object();

    //     if (this == most_massive_object)
    //         return;

    //     double distance_from_object = get_distance(this->m_pos, most_massive_object->m_pos);
    //     sf::Text dist("Distance from the " + most_massive_object->m_name + ": " + std::to_string(distance_from_object / AU) + " AU", GUI::font, 15);
    //     dist.setFillColor(sf::Color::White);
    //     bounds = dist.getLocalBounds();
    //     dist.setPosition(sf::Vector2f(target.getSize().x - bounds.width - 10, 10 + 25 * 3));
    //     target.draw(dist);

    //     sf::Text ap("Object apogee: " + std::to_string(m_ap / AU) + " AU", GUI::font, 15);
    //     ap.setFillColor(sf::Color::White);
    //     bounds = ap.getLocalBounds();
    //     ap.setPosition(sf::Vector2f(target.getSize().x - bounds.width - 10, 10 + 25 * 4));
    //     target.draw(ap);

    //     sf::Text ap_vel("Velocity at apogee: " + std::to_string((int)m_ap_vel) + " m / s", GUI::font, 15);
    //     ap_vel.setFillColor(sf::Color::White);
    //     bounds = ap_vel.getLocalBounds();
    //     ap_vel.setPosition(sf::Vector2f(target.getSize().x - bounds.width - 10, 10 + 25 * 5));
    //     target.draw(ap_vel);

    //     sf::Text pe("Object perigee: " + std::to_string(m_pe / AU) + " AU", GUI::font, 15);
    //     pe.setFillColor(sf::Color::White);
    //     bounds = pe.getLocalBounds();
    //     pe.setPosition(sf::Vector2f(target.getSize().x - bounds.width - 10, 10 + 25 * 6));
    //     target.draw(pe);

    //     sf::Text pe_vel("Velocity at perigee: " + std::to_string((int)m_pe_vel) + " m / s", GUI::font, 15);
    //     pe_vel.setFillColor(sf::Color::White);
    //     bounds = pe_vel.getLocalBounds();
    //     pe_vel.setPosition(sf::Vector2f(target.getSize().x - bounds.width - 10, 10 + 25 * 7));
    //     target.draw(pe_vel);

    //     sf::Text orbital_period("Orbital period: " + std::to_string(m_orbit_len / 365.25) + " years", GUI::font, 15);
    //     orbital_period.setFillColor(sf::Color::White);
    //     bounds = orbital_period.getLocalBounds();
    //     orbital_period.setPosition(sf::Vector2f(target.getSize().x - bounds.width - 10, 10 + 25 * 8));
    //     target.draw(orbital_period);

    //     sf::Text orbit_eccencrity("Eccencrity: " + std::to_string(eccencrity), GUI::font, 15);
    //     orbital_period.setFillColor(sf::Color::White);
    //     bounds = orbital_period.getLocalBounds();
    //     orbital_period.setPosition(sf::Vector2f(target.getSize().x - bounds.width - 10, 10 + 25 * 9));
    //     target.draw(orbit_eccencrity);
    // }
}

void Object::draw_gui(SimulationView const& view)
{
    // FIXME: Hardcoded multiplier
    auto position = view.world_to_screen(m_pos / AU);
    //std::cout << m_name << position << std::endl;

    sf::Text text(m_name, GUI::font, 15);
    text.setPosition({static_cast<float>(position.x), static_cast<float>(position.y)});
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
