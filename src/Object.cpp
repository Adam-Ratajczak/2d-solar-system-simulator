#include "Object.hpp"
#include "Planet.hpp"
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
