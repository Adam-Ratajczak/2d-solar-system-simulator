#include "Planet.hpp"
#include "Vector2.hpp"
#include "World.hpp"
#include "gui/Slider.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>

unsigned World::object_count = 0;
bool World::collisions = true;

int main()
{
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Intro");

    World::mode = 0;

    World world {window};

    window.setFramerateLimit(60);
    
    prepare_solar(world);

    while (window.isOpen()){
        
        world.get_events();
        window.clear();
        world.update();
        world.handle_focus();
        world.draw();
        world.handle_creative_mode();
        window.display();
    }

    return EXIT_SUCCESS;
}
