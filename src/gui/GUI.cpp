#include "GUI.hpp"
#include "Button.hpp"
#include "Container.hpp"
#include "Textfield.hpp"
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

sf::Font GUI::font;

GUI::GUI(World& world, sf::RenderWindow& wnd)
: Container(nullptr), m_window(wnd), m_world(world)
{
    font.loadFromFile("../assets/Pulang.ttf");
    set_layout<BasicLayout>();

    auto container = add_widget<Container>();
    // TODO: Shrink-to-fit
    // TODO: _px, _px_o operators for int
    container->set_position({116.0_px, 10.0_px});
    container->set_size({400.0_px, 300.0_px});
    auto& layout = container->set_layout<VerticalBoxLayout>();
    layout.set_spacing(10);

    m_slider_1 = container->add_widget<Slider>(0, 100, 1);
    m_slider_1->set_display_attributes(sf::Color(200, 200, 200), sf::Color(255, 255, 255));

    m_slider_2 = container->add_widget<Slider>(0, 100, 10);
    m_slider_2->set_display_attributes(sf::Color(200, 200, 200), sf::Color(255, 255, 255));

    m_slider_3 = container->add_widget<Slider>(0, 10, 1);
    m_slider_3->set_display_attributes(sf::Color(200, 200, 200), sf::Color(255, 255, 255));
    m_slider_3->set_mode(Slider::Mode::Exponential);
    m_slider_3->set_exponent(4);

    m_text_1 = container->add_widget<Textbox>();
    m_text_1->set_display_attributes(sf::Color(255, 255, 255), sf::Color(200, 200, 200), sf::Color(150, 150, 150));
    m_text_1->set_limit(13);

    m_textfield_1 = container->add_widget<Textfield>();
    m_textfield_1->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
    m_textfield_1->set_font_size(20);
    m_textfield_1->set_content("My Textfield");
    m_textfield_1->set_alignment(Textfield::Align::RIGHT);

    auto horizontal_container = container->add_widget<Container>();
    auto& horizontal_layout = horizontal_container->set_layout<HorizontalBoxLayout>();
    horizontal_layout.set_spacing(10);

    m_slider_4 = horizontal_container->add_widget<Slider>(0, 100, 1);
    m_slider_4->set_display_attributes(sf::Color(200, 200, 200), sf::Color(255, 255, 255));

    m_text_2 = horizontal_container->add_widget<Textbox>();
    m_text_2->set_display_attributes(sf::Color(255, 255, 255), sf::Color(200, 200, 200), sf::Color(150, 150, 150));
    m_text_2->set_limit(13);

    m_text_3 = horizontal_container->add_widget<Textbox>();
    m_text_3->set_display_attributes(sf::Color(255, 255, 255), sf::Color(200, 200, 200), sf::Color(150, 150, 150));
    m_text_3->set_limit(13);

    m_text_4 = horizontal_container->add_widget<Textbox>();
    m_text_4->set_display_attributes(sf::Color(255, 255, 255), sf::Color(200, 200, 200), sf::Color(150, 150, 150));
    m_text_4->set_limit(13);

    horizontal_container->get_layout()->set_multipliers({0.5, 1.5, 0.7, 1.3});
    
    m_create_button = add_widget<Button>(load_image("../assets/createButton.png"));
    m_create_button->set_position({10.0_px, 10.0_px});
    m_create_button->set_size({96.0_px, 96.0_px}); // TODO: Preferred size
    m_create_button->on_change = [container = container.get()](bool state) {
        container->set_visible(state);
    };
    m_create_button->set_active(false);
    
    auto sizes = m_window.getSize();
    m_home_button = add_widget<Button>(load_image("../assets/homeButton.png"));
    m_home_button->set_position({10.0_px_o, 10.0_px_o});
    m_home_button->set_size({96.0_px, 96.0_px}); // TODO: Preferred size
    m_home_button->on_change = [&world](bool state) {
        world.view.set_offset(sf::Vector2f(0, 0));
        world.view.set_zoom(1);
    };
}

void GUI::relayout()
{
    // TODO: Don't do this in every tick.
    set_raw_size(sf::Vector2f(m_window.getSize()));

    Container::relayout();
}
