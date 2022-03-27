#include "GUI.hpp"
#include "Button.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>

static sf::Image load_image()
{
    // TODO: Error handling
    sf::Image image;
    image.loadFromFile("../assets/createButton.png");
    return image;
}

GUI::GUI(sf::RenderWindow& window)
: m_window(window)
, m_slider_1(*this, 0, 100, 1)
, m_slider_2(*this, 0, 100, 1)
, m_slider_3(*this, 0, 100, 1)
, m_text_1(*this, sf::IntRect(50, 400, 200, 30))
, m_create_button(*this, {}, load_image(), 0.4)
{
    m_slider_1.set_display_attributes(sf::Color(200, 200, 200), sf::Color(255, 255, 255));
    m_slider_2.set_display_attributes(sf::Color(200, 200, 200), sf::Color(255, 255, 255));
    m_slider_3.set_display_attributes(sf::Color(200, 200, 200), sf::Color(255, 255, 255));

    m_text_1.set_display_attributes(sf::Color(255, 255, 255), sf::Color(200, 200, 200), sf::Color(150, 150, 150));
    m_text_1.set_limit(13);

    m_slider_1.set_position(sf::Vector2f(50, 100));
    m_slider_2.set_position(sf::Vector2f(50, 200));
    m_slider_3.set_position(sf::Vector2f(50, 300));

    m_slider_1.set_size(200, 3);
    m_slider_2.set_size(200, 3);
    m_slider_3.set_size(200, 3);

    m_widgets.push_back(&m_create_button);
    m_widgets.push_back(&m_text_1);
}

void GUI::draw()
{
    for(auto* w: m_widgets)
        w->relayout_if_needed();

    // TODO: Proper widget hierarchy
    m_create_button.draw(m_window);

    if(m_create_button.is_active())
    {
        m_slider_1.draw(m_window);
        m_slider_2.draw(m_window);
        m_slider_3.draw(m_window);
        m_text_1.draw(m_window);
    }
}

void GUI::get_events(sf::Event& event)
{
    // FIXME: Widget hierarchy (must be done anyway for layout system)
    m_create_button.handle_event(event);

    if(m_create_button.is_active())
    {
        m_slider_1.handle_event(event);
        m_slider_2.handle_event(event);
        m_slider_3.handle_event(event);
        m_text_1.handle_event(event);
    }
}
