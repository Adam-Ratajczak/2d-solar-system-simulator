#pragma once

#include "Constants.hpp"
#include "Moon.hpp"
#include "Planet.hpp"
#include "Vector2.hpp"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <cstdint>
#include <list>
#include <string>

class World{
private:
    std::list<Planet> planet_list;
    sf::Event event;
    Planet* focusing;

    // TODO: Move it to GUI.
    unsigned clicks = 0, speed = 1;
    sf::Color color;
    bool edit = false, dragging = false, focused = false;
    Vector2 prev_pos, pos;
    double mass = 0, distance;

public:
    explicit World(sf::RenderWindow& window);

    static sf::Font font;
    static bool mode;
    View view;
    static unsigned object_count;
    static bool collisions;

    World();
    void get_events();
    void update();
    void draw();
    void add_planet(const Planet& planet);
    void handle_creative_mode();
    void handle_focus();
    Planet& get_planet(const std::string name);
    void add_moon(const std::string planet_label, const std::string moon_name, double mass, double radius, double distance, double vel, uint8_t color, unsigned tres);
};
