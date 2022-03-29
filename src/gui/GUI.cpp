#include "GUI.hpp"
#include "../Object.hpp"
#include "../World.hpp"
#include "Button.hpp"
#include "Container.hpp"
#include "Textbox.hpp"
#include "Textfield.hpp"
#include "ToggleButton.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cmath>
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

GUI::GUI(World& world, Application& application)
: Container(application), m_world(world)
{
    font.loadFromFile("../assets/Pulang.ttf");
    set_layout<BasicLayout>();

    m_simulation_view = add_widget<SimulationView>(world);
    m_simulation_view->set_size({ { 100, Length::Percent }, { 100, Length::Percent } });
    m_simulation_view->on_coord_measure = [&](sf::Vector2f pos)
    {
        // TODO: Add widget enabled state and use it instead.
        m_add_object_button->set_visible(true);
        m_new_object_pos = pos;
    };

    m_world.m_simulation_view = m_simulation_view.get();

    auto container = add_widget<Container>();
    // TODO: Shrink-to-fit
    container->set_position({ 100.0_px, 10.0_px });
    container->set_size({ 500.0_px, 400.0_px });
    {
        auto& layout = container->set_layout<VerticalBoxLayout>();
        layout.set_spacing(10);

        auto mass_container = container->add_widget<Container>();
        {
            auto& mass_layout = mass_container->set_layout<HorizontalBoxLayout>();
            mass_layout.set_spacing(10);

            auto mass_textfield = mass_container->add_widget<Textfield>();
            mass_textfield->set_size({ 150.0_px, Length::Auto });
            mass_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
            mass_textfield->set_font_size(20);
            mass_textfield->set_content("Mass: ");
            mass_textfield->set_alignment(Textfield::Align::CenterLeft);

            m_mass_textbox = mass_container->add_widget<Textbox>();
            m_mass_textbox->set_display_attributes(sf::Color(255, 255, 255), sf::Color(200, 200, 200), sf::Color(150, 150, 150));
            m_mass_textbox->set_limit(6);
            m_mass_textbox->set_content("1.0");

            auto mass_value_container = mass_container->add_widget<Container>();
            mass_value_container->set_layout<HorizontalBoxLayout>();
            {
                auto mass_exponent_textfield = mass_value_container->add_widget<Textfield>();
                mass_exponent_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
                mass_exponent_textfield->set_font_size(20);
                mass_exponent_textfield->set_content(" * 10 ^ ");
                mass_exponent_textfield->set_alignment(Textfield::Align::CenterLeft);

                m_mass_exponent_textbox = mass_value_container->add_widget<Textbox>();
                m_mass_exponent_textbox->set_display_attributes(sf::Color(255, 255, 255), sf::Color(200, 200, 200), sf::Color(150, 150, 150));
                m_mass_exponent_textbox->set_limit(2);
                m_mass_exponent_textbox->set_content("1");

                auto mass_unit_textfield = mass_value_container->add_widget<Textfield>();
                mass_unit_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
                mass_unit_textfield->set_font_size(20);
                mass_unit_textfield->set_content(" kg ");
                mass_unit_textfield->set_alignment(Textfield::Align::Center);
            }
            mass_layout.set_multipliers({ 5.f / 3, 5.f / 3, 5.f / 9, 5.f / 9, 5.f / 9 });
        }

        auto radius_container = container->add_widget<Container>();
        {
            auto& radius_layout = radius_container->set_layout<HorizontalBoxLayout>();
            radius_layout.set_spacing(10);

            auto radius_textfield = radius_container->add_widget<Textfield>();
            radius_textfield->set_size({ 150.0_px, Length::Auto });
            radius_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
            radius_textfield->set_font_size(20);
            radius_textfield->set_content("Radius: ");
            radius_textfield->set_alignment(Textfield::Align::CenterLeft);

            m_radius_textbox = radius_container->add_widget<Textbox>();
            m_radius_textbox->set_display_attributes(sf::Color(255, 255, 255), sf::Color(200, 200, 200), sf::Color(150, 150, 150));
            m_radius_textbox->set_limit(8);
            m_radius_textbox->set_content("1.0");

            auto radius_unit_textfield = radius_container->add_widget<Textfield>();
            radius_unit_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
            radius_unit_textfield->set_font_size(20);
            radius_unit_textfield->set_content(" km ");
            radius_unit_textfield->set_alignment(Textfield::Align::Center);
        }
        auto velocity_container = container->add_widget<Container>();
        {
            auto& velocity_layout = velocity_container->set_layout<HorizontalBoxLayout>();
            velocity_layout.set_spacing(10);

            auto velocity_textfield = velocity_container->add_widget<Textfield>();
            velocity_textfield->set_size({ 150.0_px, Length::Auto });
            velocity_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
            velocity_textfield->set_font_size(20);
            velocity_textfield->set_content("Velocity: ");
            velocity_textfield->set_alignment(Textfield::Align::CenterLeft);

            m_velocity_textbox = velocity_container->add_widget<Textbox>();
            m_velocity_textbox->set_display_attributes(sf::Color(255, 255, 255), sf::Color(200, 200, 200), sf::Color(150, 150, 150));
            m_velocity_textbox->set_limit(8);
            m_velocity_textbox->set_content("1.0");

            auto velocity_unit_textfield = velocity_container->add_widget<Textfield>();
            velocity_unit_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
            velocity_unit_textfield->set_font_size(20);
            velocity_unit_textfield->set_content(" m / s ");
            velocity_unit_textfield->set_alignment(Textfield::Align::Center);
        }
        auto direction_container = container->add_widget<Container>();
        {
            auto& direction_layout = direction_container->set_layout<HorizontalBoxLayout>();
            direction_layout.set_spacing(10);

            auto direction_textfield = direction_container->add_widget<Textfield>();
            direction_textfield->set_size({ 150.0_px, Length::Auto });
            direction_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
            direction_textfield->set_font_size(20);
            direction_textfield->set_content("Direction: ");
            direction_textfield->set_alignment(Textfield::Align::CenterLeft);

            m_direction_slider = direction_container->add_widget<Slider>(0, 360, 1);
            m_direction_slider->set_display_attributes(sf::Color(200, 200, 200), sf::Color(255, 255, 255));

            auto direction_unit_textfield = direction_container->add_widget<Textfield>();
            direction_unit_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
            direction_unit_textfield->set_font_size(20);
            direction_unit_textfield->set_content(std::to_string((int)m_direction_slider->get_value()) + " [deg]");
            direction_unit_textfield->set_alignment(Textfield::Align::Center);

            m_direction_slider->on_change = [direction_unit_textfield](double value)
            {
                direction_unit_textfield->set_content(std::to_string((int)value) + " [deg]");
            };
        }
        auto main_color_container = container->add_widget<Container>();
        main_color_container->set_size({ Length::Auto, 100.0_px });
        auto& main_color_layout = main_color_container->set_layout<VerticalBoxLayout>();
        main_color_layout.set_spacing(10);
        {
            auto color_label_textfield = main_color_container->add_widget<Textfield>();
            color_label_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
            color_label_textfield->set_font_size(20);
            color_label_textfield->set_content("COLOR");
            color_label_textfield->set_alignment(Textfield::Align::Center);

            auto color_r_container = main_color_container->add_widget<Container>();
            {
                auto& color_r_layout = color_r_container->set_layout<HorizontalBoxLayout>();
                color_r_layout.set_spacing(10);

                auto color_r_textfield = color_r_container->add_widget<Textfield>();
                color_r_textfield->set_size({ 150.0_px, Length::Auto });
                color_r_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
                color_r_textfield->set_font_size(20);
                color_r_textfield->set_content("Red: ");
                color_r_textfield->set_alignment(Textfield::Align::CenterLeft);

                m_color_r_slider = color_r_container->add_widget<Slider>(0, 255, 1);
                m_color_r_slider->set_display_attributes(sf::Color(200, 200, 200), sf::Color(255, 255, 255));

                auto color_r_value_textfield = color_r_container->add_widget<Textfield>();
                color_r_value_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
                color_r_value_textfield->set_font_size(20);
                color_r_value_textfield->set_content(std::to_string((int)m_color_r_slider->get_value()));
                color_r_value_textfield->set_alignment(Textfield::Align::Center);

                m_color_r_slider->on_change = [color_r_value_textfield](double value)
                {
                    color_r_value_textfield->set_content(std::to_string((int)value));
                };
            }
            auto color_g_container = main_color_container->add_widget<Container>();
            {
                auto& color_g_layout = color_g_container->set_layout<HorizontalBoxLayout>();
                color_g_layout.set_spacing(10);

                auto color_g_textfield = color_g_container->add_widget<Textfield>();
                color_g_textfield->set_size({ 150.0_px, Length::Auto });
                color_g_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
                color_g_textfield->set_font_size(20);
                color_g_textfield->set_content("Green: ");
                color_g_textfield->set_alignment(Textfield::Align::CenterLeft);

                m_color_g_slider = color_g_container->add_widget<Slider>(0, 255, 1);
                m_color_g_slider->set_display_attributes(sf::Color(200, 200, 200), sf::Color(255, 255, 255));

                auto color_g_value_textfield = color_g_container->add_widget<Textfield>();
                color_g_value_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
                color_g_value_textfield->set_font_size(20);
                color_g_value_textfield->set_content(std::to_string((int)m_color_g_slider->get_value()));
                color_g_value_textfield->set_alignment(Textfield::Align::Center);

                m_color_g_slider->on_change = [color_g_value_textfield](double value)
                {
                    color_g_value_textfield->set_content(std::to_string((int)value));
                };
            }
            auto color_b_container = main_color_container->add_widget<Container>();
            {
                auto& color_b_layout = color_b_container->set_layout<HorizontalBoxLayout>();
                color_b_layout.set_spacing(10);

                auto color_b_textfield = color_b_container->add_widget<Textfield>();
                color_b_textfield->set_size({ 150.0_px, Length::Auto });
                color_b_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
                color_b_textfield->set_font_size(20);
                color_b_textfield->set_content("Blue: ");
                color_b_textfield->set_alignment(Textfield::Align::CenterLeft);

                m_color_b_slider = color_b_container->add_widget<Slider>(0, 255, 1);
                m_color_b_slider->set_display_attributes(sf::Color(200, 200, 200), sf::Color(255, 255, 255));

                auto color_b_value_textfield = color_b_container->add_widget<Textfield>();
                color_b_value_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
                color_b_value_textfield->set_font_size(20);
                color_b_value_textfield->set_content(std::to_string((int)m_color_b_slider->get_value()));
                color_b_value_textfield->set_alignment(Textfield::Align::Center);

                m_color_b_slider->on_change = [color_b_value_textfield](double value)
                {
                    color_b_value_textfield->set_content(std::to_string((int)value));
                };
            }
        }
        auto name_container = container->add_widget<Container>();
        {
            auto& name_layout = name_container->set_layout<HorizontalBoxLayout>();
            name_layout.set_spacing(10);

            auto name_textfield = name_container->add_widget<Textfield>();
            name_textfield->set_size({ 150.0_px, Length::Auto });
            name_textfield->set_display_attributes(sf::Color(0, 0, 0), sf::Color(0, 0, 255), sf::Color(255, 255, 255));
            name_textfield->set_font_size(20);
            name_textfield->set_content("Name: ");
            name_textfield->set_alignment(Textfield::Align::CenterLeft);

            m_name_textbox = name_container->add_widget<Textbox>();
            m_name_textbox->set_display_attributes(sf::Color(255, 255, 255), sf::Color(200, 200, 200), sf::Color(150, 150, 150));
            m_name_textbox->set_limit(20);
            m_name_textbox->set_data_type(Textbox::TEXT);
            m_name_textbox->set_content("Planet");
        }
        auto submit_container = container->add_widget<Container>();
        submit_container->set_size({ Length::Auto, 72.0_px });
        {
            auto& layout = submit_container->set_layout<HorizontalBoxLayout>();
            m_coords_button = submit_container->add_widget<Button>(load_image("../assets/coordsButton.png"));
            m_coords_button->set_size({ 72.0_px, Length::Auto }); // TODO: Preferred size
            m_coords_button->on_click = [this]()
            {
                //std::cout << "TEST" << std::endl;
                m_simulation_view->start_coords_measure();
            };

            submit_container->add_widget<Widget>(); // spacer

            m_add_object_button = submit_container->add_widget<Button>(load_image("../assets/addObjectButton.png"));
            m_add_object_button->set_size({ 72.0_px, Length::Auto }); // TODO: Preferred size
            m_add_object_button->on_click = [&world, this]()
            {
                double mass = std::stod(m_mass_textbox->get_content().toAnsiString()) * std::pow(10, std::stod(m_mass_exponent_textbox->get_content().toAnsiString()));
                double radius = std::stod(m_radius_textbox->get_content().toAnsiString()) * 1000;

                double theta = m_direction_slider->get_value() / 360 * 2 * M_PI;
                double velocity = std::stod(m_velocity_textbox->get_content().toAnsiString());
                Vector2 vel(std::cos(theta) * velocity, std::sin(theta) * velocity);

                sf::Color color(
                    m_color_r_slider->get_value(),
                    m_color_g_slider->get_value(),
                    m_color_b_slider->get_value());

                std::string name = m_name_textbox->get_content();

                Object planet(world, mass, radius, m_new_object_pos, vel, color, name, 1000);
                // FIXME: This (object_list) should be probably private.
                world.object_list.push_back(planet);

                m_simulation_view->m_measured = false;
            };
        }
    }

    m_create_button = add_widget<ToggleButton>(load_image("../assets/createButton.png"));
    m_create_button->set_position({ 10.0_px, 10.0_px });
    m_create_button->set_size({ 72.0_px, 72.0_px }); // TODO: Preferred size
    m_create_button->on_change = [container = container.get(), m_coords_button = m_coords_button.get()](bool state)
    {
        container->set_visible(state);
    };
    m_create_button->set_active(false);

    m_home_button = add_widget<Button>(load_image("../assets/homeButton.png"));
    m_home_button->set_position({ 10.0_px_o, 10.0_px_o });
    m_home_button->set_size({ 72.0_px, 72.0_px }); // TODO: Preferred size
    m_home_button->on_click = [this]()
    {
        m_simulation_view->set_offset(sf::Vector2f(0, 0));
        m_simulation_view->set_zoom(1);
    };
}

void GUI::relayout()
{
    // TODO: Don't do this in every tick.
    set_raw_size(sf::Vector2f(window().getSize()));

    Container::relayout();
}

void GUI::draw(sf::RenderWindow& window) const
{
    if(m_simulation_view->m_measured){
        double mass = std::stod(m_mass_textbox->get_content().toAnsiString()) * std::pow(10, std::stod(m_mass_exponent_textbox->get_content().toAnsiString()));
        double radius = std::stod(m_radius_textbox->get_content().toAnsiString()) * 1000;

        double theta = m_direction_slider->get_value() / 360 * 2 * M_PI;
        double velocity = std::stod(m_velocity_textbox->get_content().toAnsiString());
        Vector2 vel(std::cos(theta) * velocity, std::sin(theta) * velocity);

        sf::Color color(
            m_color_r_slider->get_value(),
            m_color_g_slider->get_value(),
            m_color_b_slider->get_value()
        );

        std::string name = m_name_textbox->get_content();

        Object planet(m_world, mass, radius, m_new_object_pos, vel, color, name, 1000);

        for(unsigned i = 0; i < 1000; i++)
            planet.update();
        
        planet.m_pos = m_new_object_pos;
        
        planet.draw(*m_simulation_view);
    }
}

void GUI::handle_event(Event& event)
{
    if(event.type() == sf::Event::Closed)
    {
        window().close();
        event.set_handled();
    }
}
