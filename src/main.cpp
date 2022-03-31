#include "Vector2.hpp"
#include "World.hpp"
#include "gui/Application.hpp"
#include "gui/GUI.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/ContextSettings.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "2D Solar System Simulator", sf::Style::Default, sf::ContextSettings(0, 0, 2));

    window.setFramerateLimit(60);

    World world;

    Application application { window };
    GUI& gui = application.set_main_widget<GUI>(world);

    prepare_solar(world);

    sf::Clock fps_clock;
    while (window.isOpen()) {
        application.handle_events();

        world.update();
        // FIXME: This should be done by Application.
        gui.do_update();

        // FIXME: View should be set by Application.
        window.setView(sf::View(sf::FloatRect({ 0, 0 }, sf::Vector2f(window.getSize()))));
        window.clear();

        // FIXME: FPS should be part of GUI, not World!
        world.set_fps(1.f / fps_clock.restart().asSeconds());
        application.draw();

        window.display();
    }

    return EXIT_SUCCESS;
}
