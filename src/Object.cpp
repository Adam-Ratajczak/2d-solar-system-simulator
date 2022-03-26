#include "Object.hpp"
#include "Object.hpp"
#include "World.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Text.hpp>
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

void Object::update(std::list<Object>& planet_list){
    Vector2 temp_vel;
    for(auto& planet : planet_list){
        if(this != &planet){
            double distance = get_distance(this->m_pos, planet.m_pos);

            if(distance < this->m_radius + planet.m_radius){
                if(!World::collisions)
                    break;
                
                std::cout << this->m_name << " - " << planet.m_name << "\n";

                double log10m1 = std::log10(this->m_mass), log10m2 = std::log10(planet.m_mass);

                if(this->m_mass < planet.m_mass){
                    this->m_name = planet.m_name;
                    this->m_trail = planet.m_trail;
                }

                double final_mass = this->m_mass + planet.m_mass, f1 = log10m1 / (log10m1 + log10m2), f2 = log10m2 / (log10m1 + log10m2);
                this->m_color.r = this->m_color.r * f1 + planet.m_color.r * f2;
                this->m_color.g = this->m_color.g * f1 + planet.m_color.g * f2;
                this->m_color.b = this->m_color.b * f1 + planet.m_color.b * f2;
                this->m_color.a = this->m_color.a * f1 + planet.m_color.a * f2;

                f1 = this->m_mass / final_mass;
                f2 = planet.m_mass / final_mass;

                this->m_vel = this->m_vel * f1 + planet.m_vel * f2;
                this->m_density = this->m_density * f1 + planet.m_density * f2;
                this->m_mass = final_mass;

                double new_area = this->m_mass / this->m_density + M_PI * this->m_radius * this->m_radius / this->m_density;
                this->m_radius = std::sqrt(new_area / M_PI);

                planet_list.remove(planet);
                break;
            }else{
                temp_vel -= attraction(planet);
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

    sf::CircleShape planet_circle(m_radius * view.scale(), 100);
    planet_circle.setOrigin(m_radius * view.scale(), m_radius * view.scale());

    if(World::mode){
        planet_circle.setRadius(m_radius / 1e7 * view.scale());
        planet_circle.setOrigin(m_radius / 1e7 * view.scale(), m_radius / 1e7 * view.scale());
    }

    planet_circle.setFillColor(m_color);
    planet_circle.setPosition(view.world_to_screen(m_pos));

    auto& target = view.target();

    if(m_trail.size() > 2)
        target.draw(trail);
    target.draw(planet_circle);

    sf::Text label(m_name, World::font, 15);
    label.setFillColor(sf::Color::White);
    label.setPosition(planet_circle.getPosition());
    auto bounds = label.getLocalBounds();
    label.setOrigin(bounds.width / 2, bounds.height / 2);

    target.draw(label);
}
