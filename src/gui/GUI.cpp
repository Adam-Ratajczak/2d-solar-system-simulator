#include "GUI.hpp"
#include "Button.hpp"
#include "Container.hpp"
#include "Textbox.hpp"
#include "Textfield.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>
#include <string>

static sf::Image load_image(std::string path)
{
    // TODO: Error handling
    sf::Image image;
    image.loadFromFile(path);
    return image;
}

sf::Font GUI::font;

GUI::GUI(World& world)
: Container(nullptr), m_world(world), m_window(world.view.target())
{
    font.loadFromFile("../assets/Pulang.ttf");
    set_layout<BasicLayout>();

    auto container = add_widget<Container>();
    // TODO: Shrink-to-fit
    container->set_position({100.0_px, 10.0_px});
    container->set_size({800.0_px, 200.0_px});
    auto& layout = container->set_layout<VerticalBoxLayout>();
    layout.set_spacing(10);

    auto mass_container = container->add_widget<Container>();
    auto& mass_layout = mass_container->set_layout<HorizontalBoxLayout>();
    mass_layout.set_spacing(10);

    auto mass_textfield = mass_container->add_widget<Textfield>();
    mass_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
    mass_textfield->set_font_size(20);
    mass_textfield->set_content("Mass: ");
    mass_textfield->set_alignment(Textfield::Align::LEFT);

    m_mass_textbox = mass_container->add_widget<Textbox>();
    m_mass_textbox->set_display_attributes(sf::Color(255, 255, 255), sf::Color(200, 200, 200), sf::Color(150, 150, 150));
    m_mass_textbox->set_limit(6);
    m_mass_textbox->set_content("1.0");

    auto mass_exponent_textfield = mass_container->add_widget<Textfield>();
    mass_exponent_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
    mass_exponent_textfield->set_font_size(20);
    mass_exponent_textfield->set_content(" * 10 ^ ");
    mass_exponent_textfield->set_alignment(Textfield::Align::CENTER);

    m_mass_exponent_textbox = mass_container->add_widget<Textbox>();
    m_mass_exponent_textbox->set_display_attributes(sf::Color(255, 255, 255), sf::Color(200, 200, 200), sf::Color(150, 150, 150));
    m_mass_exponent_textbox->set_limit(2);
    m_mass_exponent_textbox->set_content("1");

    auto mass_unit_textfield = mass_container->add_widget<Textfield>();
    mass_unit_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
    mass_unit_textfield->set_font_size(20);
    mass_unit_textfield->set_content(" kg ");
    mass_unit_textfield->set_alignment(Textfield::Align::CENTER);

    mass_layout.set_multipliers({5.f / 3, 5.f / 3, 5.f / 9, 5.f / 9, 5.f / 9});

    auto radius_container = container->add_widget<Container>();
    auto& radius_layout = radius_container->set_layout<HorizontalBoxLayout>();
    radius_layout.set_spacing(10);

    auto radius_textfield = radius_container->add_widget<Textfield>();
    radius_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
    radius_textfield->set_font_size(20);
    radius_textfield->set_content("Radius: ");
    radius_textfield->set_alignment(Textfield::Align::LEFT);

    m_radius_textbox = radius_container->add_widget<Textbox>();
    m_radius_textbox->set_display_attributes(sf::Color(255, 255, 255), sf::Color(200, 200, 200), sf::Color(150, 150, 150));
    m_radius_textbox->set_limit(8);
    m_radius_textbox->set_content("1.0");

    auto radius_unit_textfield = radius_container->add_widget<Textfield>();
    radius_unit_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
    radius_unit_textfield->set_font_size(20);
    radius_unit_textfield->set_content(" km ");
    radius_unit_textfield->set_alignment(Textfield::Align::CENTER);

    auto velocity_container = container->add_widget<Container>();
    auto& velocity_layout = velocity_container->set_layout<HorizontalBoxLayout>();
    velocity_layout.set_spacing(10);

    auto velocity_textfield = velocity_container->add_widget<Textfield>();
    velocity_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
    velocity_textfield->set_font_size(20);
    velocity_textfield->set_content("Velocity: ");
    velocity_textfield->set_alignment(Textfield::Align::LEFT);

    m_velocity_textbox = velocity_container->add_widget<Textbox>();
    m_velocity_textbox->set_display_attributes(sf::Color(255, 255, 255), sf::Color(200, 200, 200), sf::Color(150, 150, 150));
    m_velocity_textbox->set_limit(8);
    m_velocity_textbox->set_content("1.0");

    auto velocity_unit_textfield = velocity_container->add_widget<Textfield>();
    velocity_unit_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
    velocity_unit_textfield->set_font_size(20);
    velocity_unit_textfield->set_content(" m / s ");
    velocity_unit_textfield->set_alignment(Textfield::Align::CENTER);

    auto direction_container = container->add_widget<Container>();
    auto& direction_layout = direction_container->set_layout<HorizontalBoxLayout>();
    direction_layout.set_spacing(10);

    auto direction_textfield = direction_container->add_widget<Textfield>();
    direction_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
    direction_textfield->set_font_size(20);
    direction_textfield->set_content("Direction: ");
    direction_textfield->set_alignment(Textfield::Align::LEFT);

    m_direction_slider = direction_container->add_widget<Slider>(0, 360, 1);
    m_direction_slider->set_display_attributes(sf::Color(200, 200, 200), sf::Color(255, 255, 255));
    m_direction_slider->set_position({50.0_px, 100.0_px});
    m_direction_slider->set_size({200.0_px, 15.0_px});

    auto direction_unit_textfield = direction_container->add_widget<Textfield>();
    direction_unit_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
    direction_unit_textfield->set_font_size(20);
    direction_unit_textfield->set_content(std::to_string((int)m_direction_slider->get_value()) +  " [deg] ");
    direction_unit_textfield->set_alignment(Textfield::Align::CENTER);

    auto main_color_container = container->add_widget<Container>();
    auto& main_color_layout = main_color_container->set_layout<VerticalBoxLayout>();
    main_color_layout.set_spacing(10);

    auto color_label_textfield = main_color_container->add_widget<Textfield>();
    color_label_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
    color_label_textfield->set_font_size(20);
    color_label_textfield->set_content("COLOR");
    color_label_textfield->set_alignment(Textfield::Align::CENTER);

    auto color_r_container = main_color_container->add_widget<Container>();
    auto& color_r_layout = color_r_container->set_layout<HorizontalBoxLayout>();
    color_r_layout.set_spacing(10);

    auto color_r_textfield = color_r_container->add_widget<Textfield>();
    color_r_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
    color_r_textfield->set_font_size(20);
    color_r_textfield->set_content("Red: ");
    color_r_textfield->set_alignment(Textfield::Align::LEFT);

    m_color_r_slider = color_r_container->add_widget<Slider>(0, 255, 1);
    m_color_r_slider->set_display_attributes(sf::Color(200, 200, 200), sf::Color(255, 255, 255));
    m_color_r_slider->set_position({50.0_px, 100.0_px});
    m_color_r_slider->set_size({200.0_px, 15.0_px});

    auto color_r_value_textfield = color_r_container->add_widget<Textfield>();
    color_r_value_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
    color_r_value_textfield->set_font_size(20);
    color_r_value_textfield->set_content(std::to_string((int)m_color_r_slider->get_value()));
    color_r_value_textfield->set_alignment(Textfield::Align::CENTER);

    auto color_g_container = main_color_container->add_widget<Container>();
    auto& color_g_layout = color_g_container->set_layout<HorizontalBoxLayout>();
    color_g_layout.set_spacing(10);

    auto color_g_textfield = color_g_container->add_widget<Textfield>();
    color_g_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
    color_g_textfield->set_font_size(20);
    color_g_textfield->set_content("Green: ");
    color_g_textfield->set_alignment(Textfield::Align::LEFT);

    m_color_g_slider = color_g_container->add_widget<Slider>(0, 255, 1);
    m_color_g_slider->set_display_attributes(sf::Color(200, 200, 200), sf::Color(255, 255, 255));
    m_color_g_slider->set_position({50.0_px, 100.0_px});
    m_color_g_slider->set_size({200.0_px, 15.0_px});

    auto color_g_value_textfield = color_g_container->add_widget<Textfield>();
    color_g_value_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
    color_g_value_textfield->set_font_size(20);
    color_g_value_textfield->set_content(std::to_string((int)m_color_g_slider->get_value()));
    color_g_value_textfield->set_alignment(Textfield::Align::CENTER);

    auto color_b_container = main_color_container->add_widget<Container>();
    auto& color_b_layout = color_b_container->set_layout<HorizontalBoxLayout>();
    color_b_layout.set_spacing(10);

    auto color_b_textfield = color_b_container->add_widget<Textfield>();
    color_b_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
    color_b_textfield->set_font_size(20);
    color_b_textfield->set_content("Blue: ");
    color_b_textfield->set_alignment(Textfield::Align::LEFT);

    m_color_b_slider = color_b_container->add_widget<Slider>(0, 255, 1);
    m_color_b_slider->set_display_attributes(sf::Color(200, 200, 200), sf::Color(255, 255, 255));
    m_color_b_slider->set_position({50.0_px, 100.0_px});
    m_color_b_slider->set_size({200.0_px, 15.0_px});

    auto color_b_value_textfield = color_b_container->add_widget<Textfield>();
    color_b_value_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
    color_b_value_textfield->set_font_size(20);
    color_b_value_textfield->set_content(std::to_string((int)m_color_b_slider->get_value()));
    color_b_value_textfield->set_alignment(Textfield::Align::CENTER);

    layout.set_multipliers({1, 1, 1, 1, 3});
    
    m_create_button = add_widget<Button>(load_image("../assets/createButton.png"));
    m_create_button->set_position({10.0_px, 10.0_px});
    m_create_button->set_size({72.0_px, 72.0_px}); // TODO: Preferred size
    m_create_button->on_change = [container = container.get()](bool state) {
        container->set_visible(state);
    };
    m_create_button->set_active(false);
    
    auto sizes = m_window.getSize();
    m_home_button = add_widget<Button>(load_image("../assets/homeButton.png"));
    m_home_button->set_position({10.0_px_o, 10.0_px_o});
    m_home_button->set_size({72.0_px, 72.0_px}); // TODO: Preferred size
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
