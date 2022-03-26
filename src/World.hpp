#pragma once

#include "Constants.hpp"
#include "Object.hpp"
#include "Vector2.hpp"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <cstdint>
#include <list>
#include <string>

class World{
private:
    std::list<Object> planet_list;
    sf::Event event;
    Object* focused_planet = nullptr;

    // TODO: Move it to GUI.
    unsigned clicks = 0, speed = 1;
    Vector2 prev_pos;

public:
    explicit World(sf::RenderWindow& window);

    static sf::Font font;
    static bool mode;
    View view;
    static unsigned object_count;
    static bool collisions;
    static bool dragging;

    World();
    void get_events(sf::Event& event);
    void update();
    void draw();
    void add_planet(const Object& planet);
    void handle_focus();
    Object& get_planet(const std::string name);
};

void prepare_solar(World& world);
