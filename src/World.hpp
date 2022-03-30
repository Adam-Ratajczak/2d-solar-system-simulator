#pragma once

#include "Constants.hpp"
#include "Object.hpp"
#include "Vector2.hpp"
#include "gui/Date.hpp"
#include "gui/SimulationView.hpp"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <cstdint>
#include <list>
#include <string>

class World {
public:
    World();

    Object* most_massive_object = nullptr;
    Date date;
    unsigned object_count = 0;
    bool collisions = false;
    bool reverse = false;
    std::list<Object> object_list;
    SimulationView* m_simulation_view {};

    // FIXME: Make it a signed float and private.
    unsigned speed = 1;

    void update();
    void draw(SimulationView const& view);
    void add_object(const Object& object);
    Object& get_object(const std::string name);

    template<class C>
    void for_each_object(C callback) {
        for (auto& it : object_list)
            callback(it);
    }

    // FIXME: This should be in GUI.
    void set_fps(float fps) { m_fps = fps; }

private:
    float m_fps = 60;
};

void prepare_solar(World& world);
