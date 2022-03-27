#pragma once

#include "Button.hpp"
#include "Slider.hpp"
#include "Textbox.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <random>

#include "Widget.hpp"

class GUI
{
    sf::RenderWindow& m_window;

    Button m_create_button;
    Slider m_slider_1;
    Slider m_slider_2;
    Slider m_slider_3;
    Textbox m_text_1;

    std::vector<Widget*> m_widgets;

public:
    GUI(sf::RenderWindow& window);
    void draw();
    void get_events(sf::Event& event);
};
