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

    Application application { window };

    World world;
    GUI gui { world, application };

    prepare_solar(world);

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            Event gui_event(event);
            gui.do_handle_event(gui_event);
        }

        world.update();
        gui.do_update();

        // FIXME: View should be set by GUI.
        window.setView(sf::View(sf::FloatRect({ 0, 0 }, sf::Vector2f(window.getSize()))));
        window.clear();

        gui.relayout_and_draw(window);
        // FIXME: GUI object should be part of Application.
        application.draw(window);

        window.display();
    }

    return EXIT_SUCCESS;
}
