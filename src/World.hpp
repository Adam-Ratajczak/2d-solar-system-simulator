#pragma once

#include "Object.hpp"
#include "ObjectHistory.hpp"
#include "math/Vector3.hpp"
#include "pyssa/WrappedObject.hpp"
#include "util/Constants.hpp"
#include "util/SimulationClock.hpp"

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <list>
#include <memory>
#include <string>

class SimulationView;

class World : public PySSA::WrappedObject<World> {
public:
    World();

    World(World const& other) = delete;
    World& operator=(World const& other) = delete;
    World(World&& other) = default;
    World& operator=(World&& other) = default;
    SimulationView* m_simulation_view {};

    void update(int steps);
    void draw(SimulationView const& view) const;
    void add_object(std::unique_ptr<Object>);
    void reset();
    Object* get_object_by_name(std::string const& name);

    Util::SimulationClock::time_point date() const { return m_date; }

    template<class C>
    void for_each_object(C callback) {
        for (auto& it : m_object_list)
            callback(*it);
    }

    void clone_for_forward_simulation(World& new_world) const;

    int simulation_seconds_per_tick() const { return m_simulation_seconds_per_tick; }
    void set_simulation_seconds_per_tick(int s) { m_simulation_seconds_per_tick = s; }

    void reset_all_trails();

    static void setup_python_bindings(TypeSetup);
    static constexpr char const* PythonClassName = "World";

    void delete_object_by_ptr(Object* ptr);
    std::unique_ptr<Object>& find_object_by_ptr(Object* ptr);
    std::unique_ptr<Object>& last_object(){return m_object_list.back();}

private:
    Util::SimulationClock::time_point m_date;
    ObjectHistory m_object_history;
    std::list<std::unique_ptr<Object>> m_object_list;
    int m_simulation_seconds_per_tick = 60 * 60 * 12; // 12 Hours / half a day
    bool m_is_forward_simulated = false;

    // FIXME: (on WrappedObject side) Allow const-qualified members
    PySSA::Object python_add_object(PySSA::Object const& args, PySSA::Object const& kwargs);
    PySSA::Object python_get_object_by_name(PySSA::Object const& args, PySSA::Object const& kwargs);
    PySSA::Object python_get_simulation_seconds_per_tick() const;
    bool python_set_simulation_seconds_per_tick(PySSA::Object const&);

    friend std::ostream& operator<<(std::ostream& out, World const&);
};

void prepare_solar(World& world);
