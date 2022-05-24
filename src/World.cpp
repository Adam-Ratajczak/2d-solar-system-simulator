// keep first!
#include <GL/glew.h>

#include "ConfigLoader.hpp"
#include "Object.hpp"
#include "SimulationView.hpp"
#include "World.hpp"
#include "essagui/EssaGUI.hpp"
#include "pyssa/Object.hpp"
#include "pyssa/TupleParser.hpp"

#include <EssaGUI/util/SimulationClock.hpp>
#include <EssaGUI/util/Vector3.hpp>
#include <GL/gl.h>
#include <SFML/Graphics.hpp>
#include <cassert>
#include <iostream>
#include <memory>
#include <sstream>

using namespace std::chrono_literals;

World::World()
    : m_date(Util::SimulationTime::create(1990, 4, 20)) {
    m_start_date = m_date;
}

void World::add_object(std::unique_ptr<Object> object) {
    object->m_world = this;
    object->m_creation_date = m_date;
    m_object_list.push_back(std::move(object));

    if (m_object_history.set_time(m_date))
        m_object_history.clear_history(m_object_history.get_pos());

    m_object_list.remove_if([&](std::unique_ptr<Object>& obj) {
        return obj->creation_date() > m_date;
    });
}

void World::set_forces() {
    for (auto& p : m_object_list) {
        if (!p->deleted())
            p->clear_forces();
    }

    for (auto it = m_object_list.begin(); it != m_object_list.end(); it++) {
        auto it2 = it;
        auto& this_object = **it;
        it2++;
        for (; it2 != m_object_list.end(); it2++) {
            if (!this_object.deleted() && !(*it2)->deleted())
                this_object.update_forces_against(**it2);
        }
    }
}

void World::update_history_and_date(bool reverse) {
    if (!m_is_forward_simulated) {
        m_object_history.set_time(m_date);
        std::unique_ptr<Object>& last_created = m_object_list.back();

        if (!reverse) {
            m_date += Util::SimulationClock::duration(m_simulation_seconds_per_tick);

            if (m_object_history.size() > 0) {
                if (last_created->creation_date() <= m_date) {
                    m_object_list.push_back(std::move(m_object_history.pop_from_entries()));
                }
            }
        }
        else {
            m_date -= Util::SimulationClock::duration(m_simulation_seconds_per_tick);

            if (m_object_history.size() > 0) {
                if (last_created->creation_date() > m_date) {
                    m_object_history.push_to_entry(std::move(m_object_list.back()));
                    m_object_list.pop_back();
                }
            }
        }
    }
}

void World::update(int steps) {
    assert(steps != 0);
    bool reverse = steps < 0;

    for (unsigned i = 0; i < std::abs(steps); i++) {
        for (auto& obj : m_object_list)
            obj->before_update();

        update_history_and_date(reverse);

        // The algorithm used is Leapfrog KDK
        // http://courses.physics.ucsd.edu/2019/Winter/physics141/Lectures/Lecture2/volker.pdf
        double step = m_simulation_seconds_per_tick;
        double halfStep = (step / 2.0);

        double mul = (steps >= 0) ? 1 : -1;

        // calculate forces/accelerations based on current postions
        this->set_forces();

        for (auto& obj : m_object_list) // for each celestial body
        {
            if (obj->deleted())
                continue;

            // Leapfrog algorithm, step 1
            obj->set_vel(obj->vel() + halfStep * obj->acc() * mul);

            // Leapfrog algorithm, step 2
            obj->set_pos(obj->pos() + step * obj->vel() * mul);
        }

        // calculate the forces using the new positions
        this->set_forces();

        for (auto& obj : m_object_list) // for each celestial body
        {
            if (obj->deleted())
                continue;

            // Leapfrog algorithm, step 3
            obj->set_vel(obj->vel() + halfStep * obj->acc() * mul);

            obj->update(m_simulation_seconds_per_tick);

            // std::cerr << m_date.time_since_epoch().count() << ";" << obj->name() << ";" << obj->pos() << ";" << obj->vel() << ";" << std::endl;
        }

        // if ((m_date - m_start_date).count() > 24 * 60 * 60 * 50) {
        //     std::cout << "FINISHED" << std::endl;
        //     _exit(0);
        // }
    }
}

bool World::exist_object_with_name(const std::string name) const {
    for (const auto& obj : m_object_list) {
        if (obj->name() == name && !obj->deleted())
            return true;
    }

    return false;
}

void World::draw(SimulationView const& view) const {
    {
        WorldDrawScope scope { view, WorldDrawScope::ClearDepth::Yes };
        for (auto& p : m_object_list) {
            if (!p->deleted())
                p->draw(view);
        }
    }
    for (auto& p : m_object_list)
        if (!p->deleted())
            p->draw_gui(view);
}

Object* World::get_object_by_name(std::string const& name) {
    for (auto& obj : m_object_list) {
        if (obj->name() == name)
            return obj.get();
    }
    return nullptr;
}

void World::reset(ConfigLoader* loader) {
    m_object_list.clear();
    m_simulation_view->set_focused(nullptr);
    m_date = Util::SimulationTime::create(1990, 4, 20);
    m_object_history.clear_history(0);
    m_light_source = nullptr;
    if (loader)
        loader->load(*this);
}

void World::reset_all_trails() {
    for_each_object([](Object& o) { o.trail().reset(); });
}

void World::delete_object_by_ptr(Object* ptr) {
    m_object_list.remove_if([ptr](std::unique_ptr<Object>& obj) { return obj.get() == ptr; });
    m_simulation_view->set_focused(nullptr);

    for (auto& o : m_object_list) {
        if (o->most_attracting_object() == ptr)
            o->delete_most_attracting_object();
    }

    if (m_light_source == ptr)
        m_light_source = nullptr;
}

std::unique_ptr<Object>& World::find_object_by_ptr(Object* ptr) {
    for (auto& o : m_object_list) {
        if (o.get() == ptr)
            return o;
    }

    return m_object_list.back();
}

void World::clone_for_forward_simulation(World& new_world) const {
    new_world = World();
    new_world.m_is_forward_simulated = true;
    new_world.m_simulation_view = m_simulation_view;
    for (auto& object : m_object_list) {
        if (!object->deleted())
            new_world.add_object(object->clone_for_forward_simulation());
    }
}

std::ostream& operator<<(std::ostream& out, World const& world) {
    return out << "World (" << world.m_object_list.size() << " objects)";
}

void World::setup_python_bindings(TypeSetup adder) {
    adder.add_method<&World::python_get_object_by_name>("get_object_by_name", "Returns an object that has the name given in argument.");
    adder.add_method<&World::python_add_object>("add_object", "Adds an object to the World.");
    adder.add_attribute<&World::python_get_simulation_seconds_per_tick, &World::python_set_simulation_seconds_per_tick>("simulation_seconds_per_tick",
        "Sets how much simulation seconds passes per tick");
}

PySSA::Object World::python_add_object(PySSA::Object const& args, PySSA::Object const& kwargs) {
    Object* object = nullptr;
    if (!PySSA::parse_arguments(args, kwargs, "O!", PySSA::Arg::Arg { PySSA::Arg::CheckedType<Object> { object }, "object" }))
        return {};
    object->python_stop_owning();
    add_object(std::unique_ptr<Object>(object));
    return PySSA::Object::none();
}

PySSA::Object World::python_get_object_by_name(PySSA::Object const& args, PySSA::Object const& kwargs) {
    std::string name = "test";
    if (!PySSA::parse_arguments(args, kwargs, "s", PySSA::Arg::Arg { &name, "name" }))
        return {};

    auto object = get_object_by_name(name);
    if (object)
        return object->wrap();
    return PySSA::Object::none();
}

PySSA::Object World::python_get_simulation_seconds_per_tick() const {
    return PySSA::Object::create(m_simulation_seconds_per_tick);
}

bool World::python_set_simulation_seconds_per_tick(PySSA::Object const& object) {
    auto maybe_value = object.as_int();
    if (!maybe_value.has_value())
        return false;
    m_simulation_seconds_per_tick = maybe_value.value();
    return true;
}
