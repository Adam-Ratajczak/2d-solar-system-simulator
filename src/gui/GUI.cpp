#include "GUI.hpp"
#include "Button.hpp"
#include "Container.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>

static sf::Image load_image(std::string path)
{
    // TODO: Error handling
    sf::Image image;
    image.loadFromFile(path);
    return image;
}

GUI::GUI(sf::RenderWindow& wnd)
: Container(nullptr), m_window(wnd)
{
    set_layout<VerticalBoxLayout>();

    auto container = add_widget<Container>();
    // TODO: Shrink-to-fit
    container->set_position({100, 10});
    container->set_size({500, 400});
    auto& layout = container->set_layout<VerticalBoxLayout>();
    layout.set_spacing(10);

    m_slider_1 = container->add_widget<Slider>(0, 100, 1);
    m_slider_1->set_display_attributes(sf::Color(200, 200, 200), sf::Color(255, 255, 255));
    m_slider_1->set_position(sf::Vector2f(50, 100));
    m_slider_1->set_size({200, 15});

    m_slider_2 = container->add_widget<Slider>(0, 100, 10);
    m_slider_2->set_display_attributes(sf::Color(200, 200, 200), sf::Color(255, 255, 255));
    m_slider_2->set_position(sf::Vector2f(50, 200));
    m_slider_2->set_size({200, 15});

    m_slider_3 = container->add_widget<Slider>(0, 10, 1);
    m_slider_3->set_display_attributes(sf::Color(200, 200, 200), sf::Color(255, 255, 255));
    m_slider_3->set_position(sf::Vector2f(50, 300));
    m_slider_3->set_size({200, 15});
    m_slider_3->set_mode(Slider::Mode::Exponential);
    m_slider_3->set_exponent(4);

    m_text_1 = container->add_widget<Textbox>(sf::IntRect(50, 400, 200, 30));
    m_text_1->set_display_attributes(sf::Color(255, 255, 255), sf::Color(200, 200, 200), sf::Color(150, 150, 150));
    m_text_1->set_limit(13);

    m_textfield_1 = container->add_widget<Text>(sf::IntRect(50, 400, 200, 30));
    m_textfield_1->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
    m_textfield_1->set_font_size(20);
    m_textfield_1->set_content("My Textfield");
    m_textfield_1->set_alignment(Text::Align::RIGHT);

    auto horizontal_container = container->add_widget<Container>();
    horizontal_container->set_position({0, 0});
    horizontal_container->set_size({500, 500});
    auto& horizontal_layout = horizontal_container->set_layout<HorizontalBoxLayout>();
    horizontal_layout.set_spacing(10);

    m_slider_4 = horizontal_container->add_widget<Slider>(0, 100, 1);
    m_slider_4->set_display_attributes(sf::Color(200, 200, 200), sf::Color(255, 255, 255));
    m_slider_4->set_position(sf::Vector2f(50, 200));
    m_slider_4->set_size({200, 5});

    m_text_2 = horizontal_container->add_widget<Textbox>(sf::IntRect(50, 400, 200, 30));
    m_text_2->set_display_attributes(sf::Color(255, 255, 255), sf::Color(200, 200, 200), sf::Color(150, 150, 150));
    m_text_2->set_limit(13);
    
    m_create_button = add_widget<Button>(sf::Vector2f(), load_image("../assets/createButton.png"), 0.4);
    m_create_button->set_position({10, 10});
    m_create_button->on_change = [container = container.get()](bool state) {
        container->set_visible(state);
    };
    m_create_button->set_active(false);

}

void GUI::relayout()
{
    // TODO: Don't do this in every tick.
    set_size(sf::Vector2f(m_window.getSize()));
}
