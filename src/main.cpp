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

    World::mode = 0;

    World world {window};

    GUI gui(window);
    
    prepare_solar(world);

    while (window.isOpen()){
        sf::Event event;
        world.get_events(event);
        gui.get_events(event);
        
        window.clear();
        world.update();
        world.handle_focus();
        world.draw();

        gui.draw();

        window.display();
    }

    return EXIT_SUCCESS;
}
