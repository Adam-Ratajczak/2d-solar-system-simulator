#include "Container.hpp"

void Container::update_and_draw(sf::RenderWindow& window)
{
    Widget::update_and_draw(window);
    for(auto const& w : m_widgets)
    {
        if(w->is_visible())
            w->update_and_draw(window);
    }
}

void Container::handle_event(sf::Event& event)
{
    Widget::handle_event(event);
    for(auto const& w : m_widgets)
    {
        if(w->is_visible())
            w->handle_event(event);
    }
}
