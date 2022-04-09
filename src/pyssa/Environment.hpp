#pragma once

#include "Object.hpp"

#include <string>

class World;

namespace PySSA {

class Environment {
public:
    Environment(World& world);
    ~Environment();
    static Environment& the();
    bool run_script(std::string const& path);
    Object eval_string(std::string const&);

    std::vector<std::string> generate_exception_message() const;

private:
    World& m_world;

    PyObject* m_locals {};
    PyObject* m_globals {};

    void initialize_environment();
};

}
