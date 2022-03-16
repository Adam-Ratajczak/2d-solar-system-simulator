#include "Object.hpp"
#include "Planet.hpp"
#include "World.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Text.hpp>

Vector2 Object::attraction(const Object& other){
    double distance = get_distance(this->m_pos, other.m_pos);
    Vector2 dist = this->m_pos - other.m_pos;

    double force = G * (this->m_mass * other.m_mass) / (distance * distance);
    double theta = std::atan2(dist.y, dist.x);

    return Vector2(std::cos(theta) * force, std::sin(theta) * force);
}

bool Object::hover(Vector2 mouse_pos){
    sf::Text label(m_name, World::font, 15);
    label.setFillColor(sf::Color::White);
    label.setPosition(m_pos * World::SCALE + World::offset);
    auto bounds = label.getLocalBounds();
    auto pos = label.getPosition();

    if(mouse_pos.x > pos.x - bounds.width / 2 && mouse_pos.x < pos.x + bounds.width / 2){
        if(mouse_pos.y > pos.y - bounds.height / 2 && mouse_pos.y < pos.y + bounds.height / 2){
            return true;
        }
    }

    return false;
}

void Object::draw(sf::RenderWindow& window){
    sf::VertexArray trail(sf::LinesStrip, m_trail.size());

    unsigned i = 0;

    for(auto& l : m_trail){
        trail[i].color = m_color;
        trail[i].color.a = 128;

        trail[i].position = l * World::SCALE + World::offset;
        i++;
    }

    sf::CircleShape planet_circle(m_radius * World::SCALE, 100);
    planet_circle.setOrigin(m_radius * World::SCALE, m_radius * World::SCALE);

    if(World::mode){
        planet_circle.setRadius(m_radius / 1e7 * World::zoom);
        planet_circle.setOrigin(m_radius / 1e7 * World::zoom, m_radius / 1e7 * World::zoom);
    }

    planet_circle.setFillColor(m_color);
    planet_circle.setPosition(m_pos * World::SCALE + World::offset);

    if(m_trail.size() > 2)
        window.draw(trail);
    window.draw(planet_circle);

    sf::Text label(m_name, World::font, 15);
    label.setFillColor(sf::Color::White);
    label.setPosition(planet_circle.getPosition());
    auto bounds = label.getLocalBounds();
    label.setOrigin(bounds.width / 2, bounds.height / 2);

    window.draw(label);
}
