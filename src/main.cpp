// keep first!
#include <GL/glew.h>

#include "Vector3.hpp"
#include "World.hpp"
#include "gui/Application.hpp"
#include "gui/GUI.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    Matrix4x4d test1 = Matrix4x4d { .data = {
                                        { 4, -2, 2, 1 },
                                        { 2, 4, 3, 1 },
                                        { 3, 7, 11, 2 },
                                        { 9, -7, 1, 0 } } };
    std::cout << test1.determinant() << std::endl;
    std::cout << test1.adjoint() << std::endl;

    glewInit();
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "2D Solar System Simulator", sf::Style::Default, sf::ContextSettings(24, 0, 0));

    window.setFramerateLimit(60);

    World world;

    Application application { window };
    GUI& gui = application.set_main_widget<GUI>(world);

    prepare_solar(world);

    // Save clean OpenGL state which we will use for world rendering
    window.pushGLStates();

    sf::Clock fps_clock;
    while (window.isOpen()) {
        application.handle_events();

        // FIXME: This should be done by Application.
        gui.do_update();

        // FIXME: View should be set by Application.
        window.setView(sf::View(sf::FloatRect({ 0, 0 }, sf::Vector2f(window.getSize()))));
        window.clear();

        // FIXME: FPS should be part of GUI, not World!
        gui.simulation_view().set_fps(1.f / fps_clock.restart().asSeconds());
        application.draw();

        window.display();
    }

    return EXIT_SUCCESS;
}
