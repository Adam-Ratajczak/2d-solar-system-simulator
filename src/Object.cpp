#include "Object.hpp"
#include "Vector2.hpp"
#include "World.hpp"
#include "gui/GUI.hpp"
#include "gui/Units.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cmath>
#include <cstring>
#include <iostream>
#include <string>
#include <utility>

Object::Object(World& world, double mass, double radius, Vector2 pos, Vector2 vel, sf::Color color, std::string name, unsigned period)
    : m_world(world)
    , m_trail(period) {
    m_mass = mass;
    m_radius = radius;
    m_pos = pos;
    m_vel = vel;
    m_color = color;
    m_name = name;
    m_density = m_mass / (M_PI * radius * radius);
    m_orbit_len = period;

    m_trail.push_back(m_pos, m_vel);
}

Vector2 Object::attraction(const Object& other) {
    Vector2 dist = this->m_pos - other.m_pos;
    double force = other.m_mass / (dist.x * dist.x + dist.y * dist.y);
    Vector2 normalized_dist = dist.normalized();
    return Vector2(normalized_dist.x * force, normalized_dist.y * force);
}

bool Object::hover(SimulationView& view, Vector2 mouse_pos) {
    double dst = mouse_pos.distance_to(m_pos);
    // if(m_name == "Mercury")
    //     std::cout << dst << " <? " << 20 / view.scale() << " s=" << view.scale() << std::endl;
    return dst < 20 / view.scale();
}

void Object::update_forces(bool reverse) {
    if (m_trail.reverse_path(reverse, this))
        return;

    m_attraction_factor = Vector2();
    m_world.for_each_object([&](Object& object) {
        // TODO: Collisions
        if (this != &object)
            m_attraction_factor -= attraction(object);
    });
}

void Object::update() {
    m_vel += G * m_attraction_factor * TIMESTAMP;
    m_pos += m_vel * TIMESTAMP;

    // if(m_pos != m_trail.back())
    m_trail.push_back(m_pos, m_vel);

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
    auto& target = view.window();

    m_trail.draw(view, m_color);

    sf::CircleShape object_circle(m_radius * view.scale(), 100);
    object_circle.setOrigin(m_radius * view.scale(), m_radius * view.scale());

    object_circle.setFillColor(m_color);
    object_circle.setPosition(view.world_to_screen(m_pos));

    target.draw(object_circle);

    auto pos = object_circle.getPosition();

    sf::Text label(m_name, GUI::font, 15);
    label.setFillColor(m_is_forward_simulated ? sf::Color::Red : sf::Color::White);
    label.setPosition(pos);
    auto bounds = label.getLocalBounds();
    label.setOrigin(bounds.width / 2, bounds.height / 2);

    target.draw(label);

    // pos = view.screen_to_world(sf::Mouse::getPosition());
    // double av = (m_ap + m_pe / 2), d = get_distance(pos, World::most_massive_object->m_pos);

    // if(std::fabs(d - av) <= 2e10){
    //     auto closest = m_trail.front();
    //     double closest_distance = get_distance(closest.first, World::most_massive_object->m_pos);

    //     for(auto& t : m_trail){
    //         double dist = get_distance(t.first, World::most_massive_object->m_pos);

    //         if(dist < closest_distance){
    //             closest = t;
    //             closest_distance = dist;
    //         }
    //     }

    //     // std::cout << view.world_to_screen(closest.first) << "\n";

    //     sf::CircleShape circle(3);
    //     circle.setFillColor(m_color);
    //     circle.setOrigin(sf::Vector2f(3, 3));
    //     circle.setPosition(view.world_to_screen(closest.first));
    //     target.draw(circle);
    // }

    if (this->m_focused) {
        unsigned exponent = std::log10(m_mass);
        sf::Text mass("Mass: " + std::to_string(m_mass / std::pow(10, exponent)) + " * 10 ^ " + std::to_string(exponent) + " kg", GUI::font, 15);
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

std::unique_ptr<Object> Object::create_object_relative_to(double mass, double radius, double apogee, double perigee, bool direction, Angle theta, sf::Color color, std::string name, Angle rotation) {
    // formulae used from site: https://www.scirp.org/html/6-9701522_18001.htm
    double GM = G * this->m_mass;
    double a = (apogee + perigee) / 2;
    double b = std::sqrt(apogee * perigee);

    double T = 2 * M_PI * std::sqrt((a * a * a) / GM);
    Vector2 pos(std::cos(theta.rad()) * a, std::sin(theta.rad()) * b);
    pos = pos.rotate_vector(rotation.rad());
    pos += this->m_pos;

    auto result = std::make_unique<Object>(m_world, mass, radius * 1000, pos, Vector2(0, 0), color, name, T / (3600 * 24));
    result->m_ap = apogee;
    result->m_pe = perigee;

    result->eccencrity = std::sqrt(1 - (b * b) / (a * a));
    // std::cout << result.eccencrity << "\n";

    double velocity_constant = (2 * GM) / (apogee + perigee);

    result->m_ap_vel = std::sqrt(2 * GM / apogee - velocity_constant);
    result->m_pe_vel = std::sqrt(2 * GM / perigee - velocity_constant);
    double velocity = std::sqrt(2 * GM / get_distance(pos, this->m_pos) - velocity_constant);

    Vector2 vel(std::cos(theta.rad() + M_PI / 2) * velocity, std::sin(theta.rad() + M_PI / 2) * velocity);

    if (direction)
        vel = -vel;

    vel += this->m_vel;
    result->m_vel = vel;

    return result;
}

void Object::add_object_relative_to(double mass, double radius, double apogee, double perigee, bool direction, Angle theta, sf::Color color, std::string name, Angle rotation) {
    m_world.add_object(create_object_relative_to(mass, radius, apogee, perigee, direction, theta, color, name, rotation));
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
    auto object = std::make_unique<Object>(new_world, m_mass, m_radius, m_pos, m_vel, brightened_color(m_color), m_name, 1000);
    object->m_is_forward_simulated = true;
    return object;
}
