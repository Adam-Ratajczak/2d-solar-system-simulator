#include "Object.hpp"
#include "Object.hpp"
#include "World.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <iostream>

Vector2 Object::attraction(const Object& other){
    double distance = get_distance(this->m_pos, other.m_pos);
    Vector2 dist = this->m_pos - other.m_pos;

    double force = G * (this->m_mass * other.m_mass) / (distance * distance);
    double theta = std::atan2(dist.y, dist.x);

    return Vector2(std::cos(theta) * force, std::sin(theta) * force);
}

bool Object::hover(View& view, Vector2 mouse_pos){
    double dst = mouse_pos.distance_to(m_pos);
    // if(m_name == "Mercury")
    //     std::cout << dst << " <? " << 20 / view.scale() << " s=" << view.scale() << std::endl;
    return dst < 20 / view.scale();
}

Object::Object(
    double mass, 
    double radius, 
    Vector2 pos, 
    Vector2 vel, 
    sf::Color color,
    std::string name,
    unsigned tres){

    m_mass = mass; 
    m_radius = radius; 
    m_pos = pos; 
    m_vel = vel; 
    m_color = color;
    m_name = name;
    m_density = m_mass / (M_PI * radius * radius);
    m_tres = tres;

    m_trail.push_back(m_pos);
    World::object_count++;
}

void Object::update(std::list<Object>& object_list){
    Vector2 temp_vel;
    for(auto& object : object_list){
        if(this != &object){
            double distance = get_distance(this->m_pos, object.m_pos);

            if(distance < this->m_radius + object.m_radius){
                if(!World::collisions)
                    break;
                
                std::cout << this->m_name << " - " << object.m_name << "\n";

                double log10m1 = std::log10(this->m_mass), log10m2 = std::log10(object.m_mass);

                if(this->m_mass < object.m_mass){
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

                object_list.remove(object);
                break;
            }else{
                temp_vel -= attraction(object);
            }
        }
    }
    m_vel += temp_vel / this->m_mass * TIMESTAMP;

    m_pos += m_vel * TIMESTAMP;

    // if(m_pos != m_trail.back())
    m_trail.push_back(m_pos);
    
    if(m_trail.size() > m_tres)
        m_trail.pop_front();
            
    // std::cout << m_name << ": " << m_trail.size() << "\n"; 
}

void Object::draw(View& view){
    sf::VertexArray trail(sf::LinesStrip, m_trail.size());

    unsigned i = 0;

    for(auto& l : m_trail){
        trail[i].color = m_color;
        trail[i].color.a = 128;

        trail[i].position = view.world_to_screen(l);
        i++;
    }

    sf::CircleShape object_circle(m_radius * view.scale(), 100);
    object_circle.setOrigin(m_radius * view.scale(), m_radius * view.scale());

    if(World::mode){
        object_circle.setRadius(m_radius / 1e7 * view.scale());
        object_circle.setOrigin(m_radius / 1e7 * view.scale(), m_radius / 1e7 * view.scale());
    }

    object_circle.setFillColor(m_color);
    object_circle.setPosition(view.world_to_screen(m_pos));

    auto& target = view.target();

    if(m_trail.size() > 2)
        target.draw(trail);
    target.draw(object_circle);

    auto pos = object_circle.getPosition();

    sf::Text label(m_name, World::font, 15);
    label.setFillColor(sf::Color::White);
    label.setPosition(pos);
    auto bounds = label.getLocalBounds();
    label.setOrigin(bounds.width / 2, bounds.height / 2);

    target.draw(label);

    if(this->m_focused){
        sf::Text vel("Velocity: " + std::to_string((int)m_vel.magnitude()) + " m / s", World::font, 15);
        vel.setFillColor(sf::Color::White);
        vel.setPosition(sf::Vector2f(pos.x, pos.y + 20));
        auto bounds = vel.getLocalBounds();
        vel.setOrigin(bounds.width / 2, bounds.height / 2);

        target.draw(vel);
    }
}
