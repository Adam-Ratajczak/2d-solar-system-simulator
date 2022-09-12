// keep first!
#include <GL/glew.h>

#include "World.hpp"
#include "essagui/EssaGUI.hpp"
#include "essagui/EssaSplash.hpp"
#include "pyssa/Environment.hpp"

#include <EssaGUI/gui/Application.hpp>
#include <iostream>

int main() {
    // TODO: set framerate limit

    World world;

    GUI::Application application;
    auto& host_window = application.create_host_window({ 1000, 1000 }, "ESSA");
    auto& gui = host_window.set_main_widget<EssaGUI>(world);

    world.reset(nullptr);

    // PySSA test
#ifdef ENABLE_PYSSA
    PySSA::Environment env(world, gui.simulation_view());
    if (!env.run_script("test.py")) {
        std::cout << "Failed to execute python script :(" << std::endl;
    }
#endif

    // Display splash
    auto& splash = host_window.open_overlay<EssaSplash>(gui.settings_gui());
    splash.run();

    application.run();
    return EXIT_SUCCESS;
}
