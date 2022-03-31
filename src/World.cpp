#include "World.hpp"
#include "Object.hpp"
#include "Vector2.hpp"
#include "gui/Date.hpp"
#include "gui/GUI.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>
#include <sstream>

World::World()
: date(1990.3) {}

void World::add_object(const Object& object)
{
    object_list.push_back(object);

    if(most_massive_object == nullptr || most_massive_object->m_mass < object_list.back().m_mass)
        most_massive_object = &object_list.back();
    // std::cout << most_massive_object->m_name << "\n";
}

void World::update()
{
    for(unsigned i = 0; i < speed; i++)
    {
        if(!reverse)
            date.day_count++;
        else
            date.day_count--;

        for(auto& p : object_list)
        {
            p.update_forces();
        }

        for(auto& p : object_list)
        {
            p.update();
        }
    }
}

void World::draw(SimulationView const& view)
{
    for(auto& p : object_list)
    {
        p.draw(view);
    }

    // std::cout << date.to_string() << "\n";
    std::ostringstream oss;
    oss << date.to_string();
    if(speed == 0)
        oss << " (Paused";
    else
        oss << " (" << speed << "x";

    if(reverse)
        oss << ", Reversed";
    oss << ")";

    sf::Text fps_text("FPS: " + std::to_string(m_fps), GUI::font, 25);
    fps_text.setFillColor(sf::Color::White);
    fps_text.setPosition(10, view.window().getSize().y - 65);
    view.window().draw(fps_text);

    sf::Text date_text(oss.str(), GUI::font, 25);
    date_text.setFillColor(sf::Color::White);
    date_text.setPosition(10, view.window().getSize().y - 35);
    view.window().draw(date_text);
}

Object& World::get_object(const std::string name)
{
    for(auto& object : object_list)
    {
        if(object.m_name == name)
            return object;
    }
    return object_list.back();
}
