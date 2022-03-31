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

    World(World const& other) = delete;
    World& operator=(World const& other) = delete;
    World(World&& other) = delete;
    World& operator=(World&& other) = delete;

    Date date;
    bool collisions = false;
    SimulationView* m_simulation_view {};

    void update(int steps);
    void draw(SimulationView const& view);
    void add_object(std::unique_ptr<Object>);
    Object* get_object_by_name(std::string const& name);

    Object* most_massive_object() const { return m_most_massive_object; }

    template<class C>
    void for_each_object(C callback) {
        for (auto& it : m_object_list)
            callback(*it);
    }

private:
    Object* m_most_massive_object = nullptr;
    std::list<std::unique_ptr<Object>> m_object_list;
};

void prepare_solar(World& world);
