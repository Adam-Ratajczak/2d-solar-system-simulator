#include "SimulationView.hpp"

#include "../World.hpp"

void SimulationView::handle_event(Event& event)
{
    if(event.type() == sf::Event::MouseButtonPressed)
    {
        if(event.event().mouseButton.button == sf::Mouse::Left)
        {
            auto world_click_pos = screen_to_world({ static_cast<double>(event.event().mouseButton.x), static_cast<double>(event.event().mouseButton.y) });
            m_prev_pos = world_click_pos;
            m_dragging = true;
            // clang-format off
            m_world.for_each_object([&](Object& object) {
                if(object.hover(*this, world_click_pos))
                {
                    if(m_focused_object != nullptr)
                        m_focused_object->m_focused = false;
                    m_focused_object = &object;
                    m_focused_object->m_focused = true;
                }
            });
            // clang-format on
        }
        else if(sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            if(m_focused_object)
            {
                m_focused_object->m_focused = false;
                m_focused_object = nullptr;
            }
        }
    }
    else if(event.type() == sf::Event::MouseWheelScrolled)
    {
        if(event.event().mouseWheelScroll.delta > 0)
            apply_zoom(1.1);
        else
            apply_zoom(1 / 1.1);
    }
    else if(event.type() == sf::Event::MouseMoved)
    {
        if(m_dragging)
        {
            Vector2 mouse_pos { static_cast<double>(event.event().mouseMove.x), static_cast<double>(event.event().mouseMove.y) };
            const sf::Vector2f newPos = screen_to_world(mouse_pos);
            const sf::Vector2f deltaPos = m_prev_pos - newPos;
            set_offset(offset() + deltaPos);
            // std::cout << "mouse_pos: " << mouse_pos << " view offset: " << view.offset() << " prev_pos: " << prev_pos << " delta_pos: " << deltaPos << std::endl;
        }
    }
    else if(event.type() == sf::Event::MouseButtonReleased)
    {
        m_dragging = false;
    }
    else if(event.type() == sf::Event::KeyReleased)
    {
        // FIXME: This is too complex.
        if(event.event().key.code == sf::Keyboard::Right)
        {
            if(!m_world.reverse)
                m_world.speed *= 2;
            else
                m_world.speed /= 2;

            if(m_world.speed == 0 && m_world.reverse)
                m_world.reverse = false;
            else if(m_world.speed == 0)
            {
                m_world.speed = 1;
                m_world.for_each_object([](Object& p)
                    { p.m_vel = -p.m_vel; });
            }
        }
        else if(event.event().key.code == sf::Keyboard::Left)
        {
            if(!m_world.reverse)
                m_world.speed /= 2;
            else
                m_world.speed *= 2;

            if(m_world.speed == 0 && !m_world.reverse)
                m_world.reverse = true;
            else if(m_world.speed == 0)
            {
                m_world.speed = 1;
                m_world.for_each_object([](Object& p)
                    { p.m_vel = -p.m_vel; });
            }
        }
    }
}

void SimulationView::draw(sf::RenderWindow& window) const
{
    m_world.draw(*this);
}

void SimulationView::update()
{
    // Handle focus
    if(m_focused_object)
        set_offset(m_focused_object->m_pos);
}
