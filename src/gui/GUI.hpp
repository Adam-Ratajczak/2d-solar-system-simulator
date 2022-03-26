#pragma once

#include "FreeButton.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

class GUI{
    FreeButton m_create_button;
public:
    GUI();

    void draw(sf::RenderWindow& window);

    void get_events(sf::Event& event);
};
