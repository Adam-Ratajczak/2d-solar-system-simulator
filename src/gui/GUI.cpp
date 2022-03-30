#include "GUI.hpp"
#include "../Object.hpp"
#include "../World.hpp"
#include "Button.hpp"
#include "Container.hpp"
#include "SimulationView.hpp"
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

sf::Font GUI::font;

static sf::Image load_image(std::string path) {
    // TODO: Error handling
    sf::Image image;
    image.loadFromFile(path);
    return image;
}
bool m_simulate = false;

GUI::GUI(World& world, Application& application)
    : Container(application)
    , m_world(world) {
    font.loadFromFile("../assets/Pulang.ttf");
    set_layout<BasicLayout>();

    m_simulation_view = add_widget<SimulationView>(world);
    m_simulation_view->set_size({ { 100, Length::Percent }, { 100, Length::Percent } });
    m_simulation_view->on_coord_measure = [&](sf::Vector2f pos) {
        // TODO: Add widget enabled state and use it instead.
        m_add_object_button->set_visible(true);
        m_new_object_pos = pos;
    };

    m_world.m_simulation_view = m_simulation_view.get();

    auto container = add_widget<Container>();
    // TODO: Shrink-to-fit
    container->set_position({ 100.0_px, 10.0_px });
    container->set_size({ 500.0_px, 500.0_px });
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

        m_create_object_from_params_gui(container);

        auto submit_container = container->add_widget<Container>();
        submit_container->set_size({ Length::Auto, 72.0_px });
        {
            auto& layout = submit_container->set_layout<HorizontalBoxLayout>();
            m_coords_button = submit_container->add_widget<Button>(load_image("../assets/coordsButton.png"));
            m_coords_button->set_size({ 72.0_px, Length::Auto }); // TODO: Preferred size
            m_coords_button->on_click = [this]() {
                // std::cout << "TEST" << std::endl;
                m_simulation_view->start_coords_measure();
            };
            m_coords_button->set_tooltip_text("Set position");

            submit_container->add_widget<Widget>(); // spacer

            m_add_object_button = submit_container->add_widget<Button>(load_image("../assets/addObjectButton.png"));
            m_add_object_button->set_size({ 72.0_px, Length::Auto }); // TODO: Preferred size
            m_add_object_button->on_click = [&world, this]() {
                // FIXME: This (object_list) should be probably private.
                world.object_list.push_back(*m_create_object_from_params());

                m_simulation_view->m_measured = false;
            };
            m_add_object_button->set_tooltip_text("Add object");
        }
    }

    m_creative_mode_button = add_widget<ToggleButton>(load_image("../assets/toggleCreativeModeButton.png"));
    m_creative_mode_button->set_position({ 10.0_px, 100.0_px });
    m_creative_mode_button->set_size({ 72.0_px, 72.0_px }); // TODO: Preferred size
    m_creative_mode_button->on_change = [](bool state) {
        std::cout << state << "\n";
    };
    m_creative_mode_button->set_active(false);
    m_creative_mode_button->set_tooltip_text("Toggle mode");

    m_create_button = add_widget<ToggleButton>(load_image("../assets/createButton.png"));
    m_create_button->set_position({ 10.0_px, 10.0_px });
    m_create_button->set_size({ 72.0_px, 72.0_px }); // TODO: Preferred size
    m_create_button->on_change = [container = container.get(), 
    m_creative_mode_button = m_creative_mode_button.get(), 
    m_simulate = &m_simulate](bool state) {
        container->set_visible(state);
        m_creative_mode_button->set_visible(state);

        if(*m_simulate)
            *m_simulate = state;
    };
    m_create_button->set_active(false);
    m_create_button->set_tooltip_text("Create new object");

    m_home_button = add_widget<Button>(load_image("../assets/homeButton.png"));
    m_home_button->set_position({ 10.0_px_o, 10.0_px_o });
    m_home_button->set_size({ 72.0_px, 72.0_px }); // TODO: Preferred size
    m_home_button->on_click = [this]() {
        m_simulation_view->set_offset(sf::Vector2f(0, 0));
        m_simulation_view->set_zoom(1);
    };
    m_home_button->set_tooltip_text("Reset coordinates");
}

void GUI::relayout() {
    // TODO: Don't do this in every tick.
    set_raw_size(sf::Vector2f(window().getSize()));

    Container::relayout();
}

void GUI::draw(sf::RenderWindow& window) const {
    if (m_simulation_view->m_measured)
        m_simulate = true;

    if (m_simulate) {
        auto m_planet_to_create = m_create_object_from_params();

        if (m_last_object.get() == nullptr || *m_planet_to_create != *m_last_object) {
            for (unsigned i = 0; i < 1000; i++)
                m_planet_to_create->update();
            m_planet_to_create->m_pos = m_new_object_pos;
            m_planet_to_create->trail().push_front(m_planet_to_create->m_pos, m_planet_to_create->m_vel);
            
            m_last_object = std::move(m_planet_to_create);
        }
        m_last_object->draw(*m_simulation_view);
    }
}

void GUI::handle_event(Event& event) {
    if (event.type() == sf::Event::Closed) {
        window().close();
        event.set_handled();
    }
}
