#include "World.hpp"
#include "Planet.hpp"
#include "Vector2.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>

World::World(){
    font.loadFromFile("Pulang.ttf");
}

void World::add_planet(const Planet &planet){
    planet_list.push_back(planet);
}

unsigned clicks = 0, speed = 1;
sf::Color color;
bool edit = false, dragging = false, focused = false;
Vector2 prev_pos, pos;
double mass = 0, distance;

bool World::mode = 0;
double World::zoom = 1;
sf::Font World::font;

void World::get_events(sf::RenderWindow &window){
    while(window.pollEvent(event)){
        if (event.type == sf::Event::Closed)
            window.close();
        else if(event.type == sf::Event::MouseButtonPressed){
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
                if(clicks == 0){
                    prev_pos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                }else if(clicks == 1){
                    color.r = rand() % 256;
                    color.g = rand() % 256;
                    color.b = rand() % 256;
                }
                if(edit)
                    clicks++;
                else{
                    dragging = true;

                    for(auto& planet : planet_list){
                        if(planet.hover(prev_pos)){
                            focusing = &planet;
                            focused = true;
                        }
                    }
                }
                
            }else if(sf::Mouse::isButtonPressed(sf::Mouse::Right)){
                if(clicks == 3){
                    prev_pos = pos;
                    clicks++;
                }else {
                    clicks = 0;
                }
                focused = false;
            }
            break;
        }else if(event.type == sf::Event::MouseWheelScrolled){
            if(clicks == 2){
                if(event.mouseWheelScroll.delta > 0){
                    mass += 0.2;
                }else if(event.mouseWheelScroll.delta < 0){
                    if(mass > 0)
                        mass -= 0.2;
                }
            }else{
                if (event.mouseWheelScroll.delta <= -1){
                    SCALE *= 1.1f;
                    zoom *= 1.1;
                }else if (event.mouseWheelScroll.delta >= 1){
                    SCALE *= .9f;
                    zoom *= 0.9;
                }
                break;
            }
        }else if(event.type == sf::Event::MouseMoved){
            if(dragging){
                const sf::Vector2f newPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                    const sf::Vector2f deltaPos = prev_pos - newPos;
                    view.setCenter(view.getCenter() + deltaPos);
                    window.setView(view);
                    prev_pos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                    break;
            }
        }else if(event.type == sf::Event::MouseButtonReleased){
            dragging = false;
            break;
        }else if(event.type == sf::Event::KeyReleased){
            if(event.key.code == sf::Keyboard::Space){
                view = window.getDefaultView();
                window.setView(view);
            }else if(event.key.code == sf::Keyboard::Right){
                speed *= 2;
                if(speed == 0)
                    speed = 1;
            }else if(event.key.code == sf::Keyboard::Left){
                speed /= 2;
            }else if(event.key.code == sf::Keyboard::C){
                edit = !edit;
            }
        }
    }
}

void World::handle_creative_mode(sf::RenderWindow& window){
    auto sizes = window.getSize();
    if(clicks == 1){
        pos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
        sf::CircleShape ring;
        distance = get_distance(prev_pos, pos);
        ring.setRadius(distance);
        ring.setFillColor(sf::Color(0, 0, 0, 0));
        ring.setOutlineColor(sf::Color::White);
        ring.setOrigin(distance, distance);
        ring.setOutlineThickness(2 * zoom);
        ring.setPointCount(100);
        ring.setPosition(prev_pos);
        window.draw(ring);
        sf::VertexArray line(sf::Lines, 2);
        line[0].color = sf::Color::White;
        line[1].color = sf::Color::White;
        line[0].position = prev_pos;
        line[1].position = pos;
        window.draw(line);

        distance /= SCALE;
        if(World::mode)
            distance /= 1e7;

        sf::Text text("Radius: " + std::to_string(distance) + " m", font, 20);
        text.setPosition(window.mapPixelToCoords(sf::Vector2i(0, 0)));
        window.draw(text);
    }else if(clicks == 2){
        sf::CircleShape ring;
        ring.setRadius(distance);
        ring.setFillColor(color);
        ring.setOrigin(distance, distance);
        ring.setPointCount(100);
        ring.setPosition(prev_pos);

        if(World::mode){
            ring.setRadius(distance / 1e7 * World::zoom);
            ring.setOrigin(distance / 1e7 * World::zoom, distance / 1e7 * World::zoom);
        }

        window.draw(ring);
        sf::Text text("Mass: 10^" + std::to_string(mass) + " kg", font, 20);
        text.setPosition(window.mapPixelToCoords(sf::Vector2i(0, 0)));
        window.draw(text);
    }else if(clicks == 3){
        collisions = false;
        pos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
        sf::CircleShape ring;
        ring.setRadius(distance);
        ring.setFillColor(color);
        ring.setOrigin(distance, distance);
        ring.setPointCount(100);
        ring.setPosition(prev_pos);
        window.draw(ring);
        sf::VertexArray line(sf::Lines, 2);
        line[0].color = sf::Color::White;
        line[1].color = sf::Color::White;
        line[0].position = prev_pos;
        line[1].position = pos;
        window.draw(line);
        Planet temp_planet(std::pow(10, mass), distance, 
            (prev_pos - offset) / SCALE,
            (pos - prev_pos) / 200, 
            color, "Temporary planet", 1000);
        std::list<Planet> temp_planets = planet_list;
        temp_planets.push_back(temp_planet);
        object_count--;
        for(unsigned i = 0; i < 1000; i++){
            for(auto& p : temp_planets){
                p.update(temp_planets);
            }
        }
        collisions = true;
        temp_planets.back().draw(window);
        sf::Text text("Velocity: " + std::to_string(((pos - prev_pos) / SCALE).magnitude()) + " m/s", font, 20);
        text.setPosition(window.mapPixelToCoords(sf::Vector2i(0, 0)));
        window.draw(text);
    }else if(clicks == 4){
        planet_list.push_back(
            Planet(std::pow(10, mass), distance, 
            (prev_pos - offset) / SCALE,
            (pos - prev_pos) / 200, 
            color, "Planet" + std::to_string(object_count), 2000));
        clicks = 0;
    }
}

void World::update(){
    for(unsigned i = 0; i < speed; i++){
        for(auto& p : planet_list){
            p.update(planet_list);
        }
    }
}

void World::draw(sf::RenderWindow &window){
    for(auto& p : planet_list)
        p.draw(window);
}

void World::handle_focus(sf::RenderWindow& window){
    if(focused){
        view.setCenter(focusing->m_pos * SCALE + offset);
        window.setView(view);
        
        for(auto& moon : focusing->moon_list){
            moon.draw(window);
            std::cout << moon.m_name << " " << moon.m_pos << "\n";
        }
    }
}

Planet& World::get_planet(const std::string name){
    for(auto& planet : planet_list){
        if(planet.m_name == name)
            return planet;
    }
    return planet_list.back();
}

void World::add_moon(const std::string planet_label, const std::string moon_name, double mass, double radius, double distance, double velocity, uint8_t color, unsigned tres){
    auto& planet = this->get_planet(planet_label);

    Vector2 pos(planet.m_pos.x - distance, planet.m_pos.y);
    Vector2 vel(planet.m_vel.x, planet.m_vel.y - velocity);

    planet.moon_list.push_back(Moon(mass, radius, pos, vel, sf::Color(color, color, color), moon_name, tres));
}
