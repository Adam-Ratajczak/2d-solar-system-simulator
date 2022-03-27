#include "World.hpp"
#include "Object.hpp"
#include "Vector2.hpp"
#include "gui/Date.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>

bool World::dragging = 0, World::reverse = false;
sf::Font World::font;
Object* World::most_massive_object = nullptr;
std::list<Object> World::object_list;

World::World(sf::RenderWindow& window)
: view(window), date(2000)
{
    font.loadFromFile("../assets/Pulang.ttf");
}

void World::add_object(const Object& object)
{
    object_list.push_back(object);

    if(most_massive_object == nullptr || most_massive_object->m_mass < object_list.back().m_mass)
        most_massive_object = &object_list.back();
    // std::cout << most_massive_object->m_name << "\n";
}

void World::get_events(sf::Event& event)
{
    auto& window = view.target();
    if(event.type == sf::Event::Closed)
        window.close();
    else if(event.type == sf::Event::MouseButtonPressed)
    {
        if(event.mouseButton.button == sf::Mouse::Left)
        {
            auto world_click_pos = view.screen_to_world({ static_cast<double>(event.mouseButton.x), static_cast<double>(event.mouseButton.y) });
            prev_pos = world_click_pos;
            dragging = true;
            for(auto& object : object_list)
            {
                if(object.hover(view, world_click_pos))
                {
                    if(focused_object != nullptr)
                        focused_object->m_focused = false;
                    focused_object = &object;
                    focused_object->m_focused = true;
                }
            }
        }
        else if(sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            focused_object->m_focused = false;
            focused_object = nullptr;
        }
    }
    else if(event.type == sf::Event::MouseWheelScrolled)
    {
        if(event.mouseWheelScroll.delta <= -1)
        {
            view.apply_zoom(1.1);
        }
        else if(event.mouseWheelScroll.delta >= 1)
        {
            view.apply_zoom(1 / 1.1);
        }
    }
    else if(event.type == sf::Event::MouseMoved)
    {
        if(dragging)
        {
            Vector2 mouse_pos { static_cast<double>(event.mouseMove.x), static_cast<double>(event.mouseMove.y) };
            const sf::Vector2f newPos = view.screen_to_world(mouse_pos);
            const sf::Vector2f deltaPos = prev_pos - newPos;
            view.set_offset(view.offset() + deltaPos);
            // std::cout << "mouse_pos: " << mouse_pos << " view offset: " << view.offset() << " prev_pos: " << prev_pos << " delta_pos: " << deltaPos << std::endl;
        }
    }
    else if(event.type == sf::Event::MouseButtonReleased)
    {
        dragging = false;
    }
    else if(event.type == sf::Event::KeyReleased)
    {
        if(event.key.code == sf::Keyboard::Space)
        {
            view.reset();
        }
        else if(event.key.code == sf::Keyboard::Right)
        {
            if(!reverse)
            {
                speed *= 2;
            }
            else
            {
                speed /= 2;
            }

            if(speed == 0 && reverse)
            {
                reverse = false;
            }
            else if(speed == 0)
            {
                speed = 1;
                for(auto& p : object_list)
                {
                    p.m_vel = -p.m_vel;
                }
            }
        }
        else if(event.key.code == sf::Keyboard::Left)
        {
            if(!reverse)
            {
                speed /= 2;
            }
            else
            {
                speed *= 2;
            }

            if(speed == 0 && !reverse)
            {
                reverse = true;
            }
            else if(speed == 0)
            {
                speed = 1;
                reverse = true;
                for(auto& p : object_list)
                {
                    p.m_vel = -p.m_vel;
                }
            }
        }
    } /* else if(event.type == sf::Event::Resized) {
         window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
     }*/
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
            p.update();
        }
    }
}

void World::draw()
{
    for(auto& p : object_list)
    {
        p.draw(view);
    }

    // std::cout << date.to_string() << "\n";

    sf::Text date_text(date.to_string(), font, 25);
    date_text.setFillColor(sf::Color::White);
    date_text.setPosition(10, view.target().getSize().y - 35);
    view.target().draw(date_text);
}

void World::handle_focus()
{
    if(focused_object)
    {
        view.set_offset(focused_object->m_pos);
    }
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
