#include "Planet.hpp"
#include "Vector2.hpp"
#include "World.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <cmath>
#include <iostream>
#include <string>

Planet::Planet(
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

void Planet::update(std::list<Planet>& planet_list){
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
