#include "Moon.hpp"
#include "World.hpp"
#include <list>

Moon::Moon(
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

void Moon::update(std::list<Moon>& moon_list){
    Vector2 temp_vel;
    for(auto& moon : moon_list)
        temp_vel += this->attraction(moon);
    
    m_vel += temp_vel / this->m_mass * TIMESTAMP;

    m_pos += m_vel * TIMESTAMP;

    // if(m_pos != m_trail.back())
    m_trail.push_back(m_pos);
    
    if(m_trail.size() > m_tres)
        m_trail.pop_front();
}
