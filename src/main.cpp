// keep first!
#include <GL/glew.h>

#include "World.hpp"
#include "essagui/EssaGUI.hpp"
#include "essagui/EssaSplash.hpp"
#include "math/Transform.hpp"
#include "pyssa/Environment.hpp"

#include <EssaGUI/gui/Application.hpp>
#include <EssaUtil/Vector3.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    glewInit();
    GUI::SFMLWindow window(sf::VideoMode::getDesktopMode(), "ESSA", sf::Style::Default, sf::ContextSettings(24, 0, 0, 3, 2));
    window.setFramerateLimit(60);

    World world;

    GUI::Application application { window };
    auto& gui = application.set_main_widget<EssaGUI>(world);

    world.reset(nullptr);

    // PySSA test
#ifdef ENABLE_PYSSA
    PySSA::Environment env(world, gui.simulation_view());
    if (!env.run_script("test.py")) {
        std::cout << "Failed to execute python script :(" << std::endl;
    }
#endif

    // Display splash
    auto& splash = GUI::Application::the().open_overlay<EssaSplash>(gui.settings_gui());
    splash.run();

    application.run();
    return EXIT_SUCCESS;
}
