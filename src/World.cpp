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

World::World(sf::RenderWindow& window)
: view(window) {
    font.loadFromFile("../assets/Pulang.ttf");
}

void World::add_planet(const Planet &planet){
    planet_list.push_back(planet);
}

bool World::mode = 0;
sf::Font World::font;

void World::get_events(){
    sf::Event event;
    auto& window = view.target();
    while(window.pollEvent(event)){
        if (event.type == sf::Event::Closed)
            window.close();
        else if(event.type == sf::Event::MouseButtonPressed){
            if(event.mouseButton.button == sf::Mouse::Left){
                auto world_click_pos = view.screen_to_world({static_cast<double>(event.mouseButton.x), static_cast<double>(event.mouseButton.y)});
                prev_pos = world_click_pos;
                dragging = true;
                for(auto& planet : planet_list){
                    if(planet.hover(view, world_click_pos)){
                        // std::cout << "yay focused: " << planet.m_name << std::endl;
                        focused_planet = &planet;
                    }
                } 
            }else if(sf::Mouse::isButtonPressed(sf::Mouse::Right)){
                focused_planet = nullptr;
            }
            break;
        }else if(event.type == sf::Event::MouseWheelScrolled){
            
            if (event.mouseWheelScroll.delta <= -1){
                view.apply_zoom(1.1);
            }else if (event.mouseWheelScroll.delta >= 1){
                view.apply_zoom(1 / 1.1);
            }
            break;
        }else if(event.type == sf::Event::MouseMoved){
            if(dragging){
                Vector2 mouse_pos {static_cast<double>(event.mouseMove.x), static_cast<double>(event.mouseMove.y)};
                const sf::Vector2f newPos = view.screen_to_world(mouse_pos);
                const sf::Vector2f deltaPos = prev_pos - newPos;
                view.set_offset(view.offset() + deltaPos);
                //std::cout << "mouse_pos: " << mouse_pos << " view offset: " << view.offset() << " prev_pos: " << prev_pos << " delta_pos: " << deltaPos << std::endl;
                break;
            }
        }else if(event.type == sf::Event::MouseButtonReleased){
            dragging = false;
            break;
        }else if(event.type == sf::Event::KeyReleased){
            if(event.key.code == sf::Keyboard::Space){
                view.reset();
            }else if(event.key.code == sf::Keyboard::Right){
                speed *= 2;
                if(speed == 0)
                    speed = 1;
            }else if(event.key.code == sf::Keyboard::Left){
                speed /= 2;
            }
        } else if(event.type == sf::Event::Resized) {
            window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
        }
    }
}

void World::update(){
    for(unsigned i = 0; i < speed; i++){
        for(auto& p : planet_list){
            p.update(planet_list);
        }
    }
}

void World::draw(){
    for(auto& p : planet_list)
    {
        // if(&p == focused_planet)
        //     std::cout << "focusing draw(): " << p.m_pos << std::endl;
        p.draw(view);
    }
}

void World::handle_focus(){
    if(focused_planet){
        for(auto& moon : focused_planet->moon_list){
            moon.draw(view);
            // std::cout << moon.m_name << " " << moon.m_pos << "\n";
        }
        // std::cout << "focusing handle_focus(): " << focused_planet->m_pos << std::endl;
        view.set_offset(focused_planet->m_pos);
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
