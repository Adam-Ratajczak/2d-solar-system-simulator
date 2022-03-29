#pragma once

#include "Constants.hpp"
#include "Object.hpp"
#include "Vector2.hpp"
#include "gui/Date.hpp"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <cstdint>
#include <list>
#include <string>

class World
{
public:
    World();

    Object* most_massive_object = nullptr;
    Date date;
    unsigned object_count = 0;
    bool collisions = false;
    bool reverse = false;
    std::list<Object> object_list;

    // FIXME: Make it a signed float and private.
    unsigned speed = 1;

    void update();
    void draw(SimulationView const& view);
    void add_object(const Object& object);
    Object& get_object(const std::string name);

    template<class C>
    void for_each_object(C callback)
    {
        for(auto& it : object_list)
            callback(it);
    }
};

void prepare_solar(World& world);
