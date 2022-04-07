// keep first!
#include <GL/glew.h>

#include "Object.hpp"
#include "Vector3.hpp"
#include "World.hpp"
#include "gui/Date.hpp"
#include "gui/GUI.hpp"
#include "gui/SimulationView.hpp"
#include <GL/gl.h>
#include <SFML/Graphics.hpp>
#include <cassert>
#include <iostream>
#include <sstream>

World::World()
    : date(1990.3) { }

void World::add_object(std::unique_ptr<Object> object) {
    if (!m_most_massive_object || m_most_massive_object->gravity_factor() < object->gravity_factor())
        m_most_massive_object = object.get();

    m_object_list.push_back(std::move(object));
}

void World::update(int steps) {
    assert(steps != 0);
    bool reverse = steps < 0;
    for (unsigned i = 0; i < std::abs(steps); i++) {
        if (!reverse)
            date.day_count++;
        else
            date.day_count--;

        for (auto& p : m_object_list)
            p->update_forces(reverse);

        for (auto& p : m_object_list)
            p->update();
    }
}

void World::draw(SimulationView const& view) const {

    WorldDrawScope scope { view };
    for (auto& p : m_object_list)
        p->draw(view);
}

Object* World::get_object_by_name(std::string const& name) {
    for (auto& obj : m_object_list) {
        if (obj->m_name == name)
            return obj.get();
    }
    return nullptr;
}

void World::clone_for_forward_simulation(World& new_world) const {
    new_world = World();
    new_world.m_simulation_view = m_simulation_view;
    for (auto& object : m_object_list)
        new_world.add_object(object->clone_for_forward_simulation(new_world));
}
