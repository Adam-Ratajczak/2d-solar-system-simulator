#include "Vector2.hpp"
#include "World.hpp"
#include "gui/GUI.hpp"
#include "gui/Root.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>

int main()
{
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "2D Solar System Simulator");

    window.setFramerateLimit(60);

    Root root { window };

    World world;
    GUI gui { world, root };

    prepare_solar(world);

    while(window.isOpen())
    {
        sf::Event event;

        while(window.pollEvent(event))
        {
            Event gui_event(event);
            gui.do_handle_event(gui_event);
        }

        world.update();
    
        // FIXME: View should be set by GUI.
        window.setView(sf::View(sf::FloatRect({ 0, 0 }, sf::Vector2f(window.getSize()))));
        window.clear();

        gui.update_and_draw(window);

        window.display();
    }

    return EXIT_SUCCESS;
}
