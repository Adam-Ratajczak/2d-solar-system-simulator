#pragma once

#include <string>

namespace PySSA
{

class Environment
{
public:
    ~Environment();
    static Environment& the();
    bool run_script(std::string const& path);

private:
    Environment();

    void initialize_environment();
};

}
