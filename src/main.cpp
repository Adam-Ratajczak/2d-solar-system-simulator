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
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Intro");
    
    window.setFramerateLimit(60);

    World world {window};

    GUI gui(window);
    
    prepare_solar(world);

    while (window.isOpen()){
        sf::Event event;

        while(window.pollEvent(event)) {
            world.get_events(event);
            gui.get_events(event);
        }

        window.setView(sf::View(sf::FloatRect({0, 0}, sf::Vector2f(window.getSize()))));
        
        window.clear();
        world.update();
        world.handle_focus();
        world.draw();

        gui.draw();

        window.display();
    }

    return EXIT_SUCCESS;
}
