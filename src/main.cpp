#include "Vector2.hpp"
#include "World.hpp"
#include "gui/GUI.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>

unsigned World::object_count = 0;
bool World::collisions = true;

int main()
{
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "2D Solar System Simulator");

    window.setFramerateLimit(60);

    World world;
    GUI gui { world, window };

    prepare_solar(world);

    while(window.isOpen())
    {
        sf::Event event;

        while(window.pollEvent(event))
            gui.handle_event(event);

        world.update();
    
        // FIXME: View should be set by GUI.
        window.setView(sf::View(sf::FloatRect({ 0, 0 }, sf::Vector2f(window.getSize()))));
        window.clear();

        gui.update_and_draw(window);

        window.display();
    }

    return EXIT_SUCCESS;
}
