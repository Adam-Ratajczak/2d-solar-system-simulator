#pragma once

#include "Moon.hpp"
#include "Planet.hpp"
#include "Vector2.hpp"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <cstdint>
#include <list>
#include <string>

const double G = 6.67408e-11;
const double AU = 149.6e6 * 1000;
const double TIMESTAMP = 3600 * 24;

class World{
private:
    std::list<Planet> planet_list;
    sf::Event event;
    Planet* focusing;

public:
    static sf::Font font;
    static double zoom;
    static bool mode;
    static sf::View view;
    static Vector2 offset;
    static unsigned object_count;
    static bool collisions;
    static double SCALE;

    World();
    void get_events(sf::RenderWindow& window);
    void update();
    void draw(sf::RenderWindow& window);
    void add_planet(const Planet& planet);
    void handle_creative_mode(sf::RenderWindow& window);
    void handle_focus(sf::RenderWindow& window);
    Planet& get_planet(const std::string name);
    void add_moon(const std::string planet_label, const std::string moon_name, double mass, double radius, double distance, double vel, uint8_t color, unsigned tres);
};