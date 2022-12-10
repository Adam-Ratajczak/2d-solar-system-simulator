#pragma once

#ifdef ENABLE_PYSSA

#include "Object.hpp"

#include <string>

class SimulationView;
class World;

namespace PySSA {

class Environment {
public:
    Environment(World& world, SimulationView& sv);
    ~Environment();
    static Environment& the();
    bool run_script(std::string const& path);
    Object eval_string(std::string const&);

    std::vector<Util::UString> generate_exception_message() const;

private:
    World& m_world;
    SimulationView& m_simulation_view;

    PyObject* m_locals {};
    PyObject* m_globals {};

    void initialize_environment();
};

}

#endif
