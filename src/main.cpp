// keep first!
#include <GL/glew.h>

#include "World.hpp"
#include "essagui/EssaGUI.hpp"
#include <EssaGUI/gui/Application.hpp>
#include "essagui/EssaSplash.hpp"
#include "math/Transform.hpp"
#include "math/Vector3.hpp"
#include "pyssa/Environment.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    glewInit();
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "ESSA", sf::Style::Default, sf::ContextSettings(24, 0, 0));
    window.setFramerateLimit(60);

    World world;

    GUI::Application application { window };
    auto& gui = application.set_main_widget<EssaGUI>(world);

    world.reset(nullptr);

    // PySSA test
    PySSA::Environment env(world, gui.simulation_view());
    if (!env.run_script("test.py")) {
        std::cout << "Failed to execute python script :(" << std::endl;
    }

    // Save clean OpenGL state which we will use for world rendering
    window.pushGLStates();

    // Display splash
    auto& splash = GUI::Application::the().open_overlay<EssaSplash>(gui.settings_gui());
    splash.run();

    application.run();
    return EXIT_SUCCESS;
}
