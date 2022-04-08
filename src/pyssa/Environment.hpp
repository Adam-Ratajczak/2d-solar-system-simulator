#pragma once

#include <string>

class World;

namespace PySSA
{

class Environment
{
public:
    Environment(World& world);
    ~Environment();
    static Environment& the();
    bool run_script(std::string const& path);

private:
    World& m_world;

    void initialize_environment();
};

}
