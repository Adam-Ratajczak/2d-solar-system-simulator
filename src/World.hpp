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

    Date date;
    bool collisions = false;
    bool reverse = false;
    SimulationView* m_simulation_view {};

    // FIXME: Make it a signed float and private.
    unsigned speed = 1;

    void update();
    void draw(SimulationView const& view);
    void add_object(std::unique_ptr<Object>);
    Object* get_object_by_name(std::string const& name);

    Object* most_massive_object() { return m_most_massive_object; }

    template<class C>
    void for_each_object(C callback) {
        for (auto& it : m_object_list)
            callback(*it);
    }

    // FIXME: This should be in GUI.
    void set_fps(float fps) { m_fps = fps; }

private:
    float m_fps = 60;

    Object* m_most_massive_object = nullptr;
    std::list<std::unique_ptr<Object>> m_object_list;
};

void prepare_solar(World& world);
