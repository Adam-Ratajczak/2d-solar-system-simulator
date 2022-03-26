#include "GUI.hpp"
#include "FreeButton.hpp"
#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>

GUI::GUI(){
    sf::Image image;
    image.loadFromFile("../assets/createButton.png");
    FreeButton buttonCreate(sf::Vector2f(0, 0), image, 3, 0.5);

    m_create_button = buttonCreate;
}

void GUI::draw(sf::RenderWindow &window){
    m_create_button.draw(window);
}

void GUI::get_events(sf::Event &event){
    m_create_button.event_listener(event, EventListening::ONCLICK);
}
