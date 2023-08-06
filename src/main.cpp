// keep first!
#include <GL/glew.h>

#include "World.hpp"
#include "essagui/EssaGUI.hpp"
#include "essagui/EssaSplash.hpp"
#include "pyssa/Environment.hpp"

#include <Essa/GUI/Application.hpp>
#include <iostream>

int main() {
    // TODO: set framerate limit

    World world;

    GUI::Application application;
    auto& host_window = application.create_host_window({ 1000, 1000 }, "ESSA");
    host_window.maximize();
    host_window.set_background_color(Util::Colors::Black);
    auto& gui = host_window.set_main_widget<EssaGUI>(world);

    world.reset({});

    // PySSA test
#ifdef ENABLE_PYSSA
    PySSA::Environment env(world, gui.simulation_view());
    if (!env.run_script("test.py")) {
        std::cout << "Failed to execute python script :(" << std::endl;
    }
#endif

    // Display splash
    // FIXME: Here basically repeating what WindowRoot does on relayout. Maybe we should:
    // - add a helper function for that (force_relayout()??)
    // - even better, ensure that layout is done as soon as possible (E.g immediately after setting main widget)
    gui.set_raw_size(host_window.size());
    gui.do_relayout();
    gui.dump(0);
    gui.mdi_host().open_overlay<EssaSplash>(gui.settings_gui());
    // splash.overlay.show_modal();

    application.run();
    return EXIT_SUCCESS;
}
