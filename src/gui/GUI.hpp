#pragma once

#include "Button.hpp"
#include "Container.hpp"
#include "Slider.hpp"
#include "Textfield.hpp"
#include "Textbox.hpp"
#include "Widget.hpp"
#include "../World.hpp"

#include <SFML/Graphics.hpp>
#include <random>

class GUI : public Container
{
    std::shared_ptr<Button> m_create_button;
    std::shared_ptr<Button> m_home_button;
    std::shared_ptr<Slider> m_slider_1;
    std::shared_ptr<Slider> m_slider_2;
    std::shared_ptr<Slider> m_slider_3;
    std::shared_ptr<Slider> m_slider_4;
    std::shared_ptr<Textbox> m_text_1;
    std::shared_ptr<Textbox> m_text_2;
    std::shared_ptr<Textfield> m_textfield_1;
    sf::RenderWindow& m_window;
    World& m_world;

public:
    GUI(World& world, sf::RenderWindow& wnd);
    static sf::Font font;

private:
    virtual void relayout() override;
};
