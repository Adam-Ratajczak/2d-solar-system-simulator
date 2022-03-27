#include "Container.hpp"

WidgetList& Layout::widgets()
{
    return m_container.m_widgets;
}

void VerticalBoxLayout::run()
{
    size_t index = 0;
    float size = (m_container.size().y - (m_spacing * (widgets().size() - 1))) / widgets().size();
    for(auto& w : widgets())
    {
        w->set_position({m_container.position().x, m_container.position().y + index * size + m_spacing * index});
        w->set_size({m_container.size().x, size});
        index++;
    }
}

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

void Container::relayout()
{
    if(!m_layout)
        return;
    m_layout->run();
}
