#pragma once

#include "FreeButton.hpp"
#include "Slider.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

class GUI{
    FreeButton m_create_button;
    Slider m_slider_1;
    Slider m_slider_2;
    Slider m_slider_3;

    sf::RenderWindow& m_window;
public:
    GUI(sf::RenderWindow& window);

    void draw();

    void get_events(sf::Event& event);
};
