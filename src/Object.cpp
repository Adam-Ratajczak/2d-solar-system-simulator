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

Object::Object(World& world, double mass, double radius, Vector2 pos, Vector2 vel, sf::Color color, std::string name, unsigned tres)
    : m_world(world)
    , m_trail(tres) {
    m_mass = mass;
    m_radius = radius;
    m_pos = pos;
    m_vel = vel;
    m_color = color;
    m_name = name;
    m_density = m_mass / (M_PI * radius * radius);
    m_tres = tres;

    m_trail.push_back(m_pos, m_vel);
    world.object_count++;
}

Vector2 Object::attraction(const Object& other) {
    Vector2 dist = this->m_pos - other.m_pos;
    double force = G * (this->m_mass * other.m_mass) / (dist.x * dist.x + dist.y * dist.y);
    Vector2 normalized_dist = dist.normalized();
    return Vector2(normalized_dist.x * force, normalized_dist.y * force);
}

bool Object::hover(SimulationView& view, Vector2 mouse_pos) {
    double dst = mouse_pos.distance_to(m_pos);
    // if(m_name == "Mercury")
    //     std::cout << dst << " <? " << 20 / view.scale() << " s=" << view.scale() << std::endl;
    return dst < 20 / view.scale();
}

void Object::update() {
    if (m_trail.reverse_path(m_world.reverse, this))
        return;

    Vector2 temp_vel;
    for (auto& object : m_world.object_list) {
        if (this != &object) {
            if (m_world.collisions) {
                double distance = get_distance(this->m_pos, object.m_pos);

                if (distance < this->m_radius + object.m_radius) {
                    // std::cout << this->m_name << " - " << object.m_name << "\n";

                    double log10m1 = std::log10(this->m_mass), log10m2 = std::log10(object.m_mass);

                    if (this->m_mass < object.m_mass) {
                        this->m_name = object.m_name;
                        this->m_trail = object.m_trail;
                    }

                    double final_mass = this->m_mass + object.m_mass, f1 = log10m1 / (log10m1 + log10m2), f2 = log10m2 / (log10m1 + log10m2);
                    this->m_color.r = this->m_color.r * f1 + object.m_color.r * f2;
                    this->m_color.g = this->m_color.g * f1 + object.m_color.g * f2;
                    this->m_color.b = this->m_color.b * f1 + object.m_color.b * f2;
                    this->m_color.a = this->m_color.a * f1 + object.m_color.a * f2;

                    f1 = this->m_mass / final_mass;
                    f2 = object.m_mass / final_mass;

                    this->m_vel = this->m_vel * f1 + object.m_vel * f2;
                    this->m_density = this->m_density * f1 + object.m_density * f2;
                    this->m_mass = final_mass;

                    double new_area = this->m_mass / this->m_density + M_PI * this->m_radius * this->m_radius / this->m_density;
                    this->m_radius = std::sqrt(new_area / M_PI);

                    m_world.object_list.remove(object);
                    break;
                }
            }
            temp_vel -= attraction(object);
        }
    }
    m_vel += temp_vel / this->m_mass * TIMESTAMP;

    m_pos += m_vel * TIMESTAMP;

    // if(m_pos != m_trail.back())
    m_trail.push_back(m_pos, m_vel);

    if (m_trail.size() > m_tres)
        m_trail.pop_front();
    m_trail.update_trail(*m_world.m_simulation_view, m_color);

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
    label.setFillColor(sf::Color::White);
    label.setPosition(pos);
    auto bounds = label.getLocalBounds();
    label.setOrigin(bounds.width / 2, bounds.height / 2);

    target.draw(label);
    double distance_from_object = get_distance(this->m_pos, m_world.most_massive_object->m_pos);

    if (m_ap < distance_from_object) {
        m_ap = distance_from_object;
        m_ap_vel = m_vel.magnitude();
    }

    if (m_pe > distance_from_object) {
        m_pe = distance_from_object;
        m_pe_vel = m_vel.magnitude();
    }

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

        if (this == m_world.most_massive_object)
            return;

        sf::Text dist("Distance from the " + m_world.most_massive_object->m_name + ": " + std::to_string(distance_from_object / AU) + " AU", GUI::font, 15);
        ;
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
    }
}

void Object::add_object(double mass, double radius, double distance, Angle theta, double velocity, sf::Color color, std::string name, unsigned tres) {
    // FIXME: add_object to... object? maybe add_object_relative_to?
    Vector2 pos(std::cos(theta.rad()) * distance, std::sin(theta.rad()) * distance);
    pos += this->m_pos;

    Vector2 vel(std::cos(theta.rad() - M_PI / 2) * velocity, std::sin(theta.rad() - M_PI / 2) * velocity);
    vel += this->m_vel;

    m_world.object_list.push_back(Object(m_world, mass, radius, pos, vel, color, name, tres));
}
