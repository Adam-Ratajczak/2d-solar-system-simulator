#include "SimulationView.hpp"

#include "../World.hpp"

void SimulationView::handle_event(Event& event)
{
    if(event.type() == sf::Event::MouseButtonPressed)
    {
        m_prev_mouse_pos = { static_cast<float>(event.event().mouseButton.x), static_cast<float>(event.event().mouseButton.y) };
        if(event.event().mouseButton.button == sf::Mouse::Left)
        {
            auto world_click_pos = screen_to_world({ static_cast<float>(event.event().mouseButton.x), static_cast<float>(event.event().mouseButton.y) });
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
            if(m_coord_measure)
            {
                m_coord_measure = false;
                m_measured = true;
                if(on_coord_measure)
                {
                    on_coord_measure(m_prev_pos);
                }
            }
            event.set_handled();
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
        m_prev_mouse_pos = { static_cast<float>(event.event().mouseMove.x), static_cast<float>(event.event().mouseMove.y) };
        if(m_dragging)
        {
            const sf::Vector2f newPos = screen_to_world(m_prev_mouse_pos);
            const sf::Vector2f deltaPos = m_prev_pos - newPos;
            set_offset(offset() + deltaPos);
            // std::cout << "mouse_pos: " << mouse_pos << " view offset: " << view.offset() << " prev_pos: " << prev_pos << " delta_pos: " << deltaPos << std::endl;
        }
    }
    else if(event.type() == sf::Event::MouseButtonReleased)
    {
        m_dragging = false;
    }
    else if(event.type() == sf::Event::KeyPressed)
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

    if(m_coord_measure)
    {
        auto sizes = size();
        sf::VertexArray lines(sf::Lines, 4);
        lines[0] = sf::Vertex{{0, static_cast<float>(m_prev_mouse_pos.y)}, sf::Color::Red};
        lines[1] = sf::Vertex{{sizes.x, static_cast<float>(m_prev_mouse_pos.y)}, sf::Color::Red};
        lines[2] = sf::Vertex{{static_cast<float>(m_prev_mouse_pos.x), 0}, sf::Color::Red};
        lines[3] = sf::Vertex{{static_cast<float>(m_prev_mouse_pos.x), sizes.y}, sf::Color::Red};
        window.draw(lines);
    }
}

void SimulationView::update()
{
    // Handle focus
    if(m_focused_object)
        set_offset(m_focused_object->m_pos);
}
