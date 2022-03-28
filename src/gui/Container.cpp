#include "Container.hpp"

#include <iostream>

WidgetList& Layout::widgets()
{
    return m_container.m_widgets;
}

void Layout::set_multipliers(std::initializer_list<float> list){
    m_multipliers.clear();

    for(auto& l : list){
        m_multipliers.push_back(l);
    }
}

void VerticalBoxLayout::run()
{
    size_t index = 0, total_size = 0;
    float size = (m_container.size().y - (m_spacing * (widgets().size() - 1))) / widgets().size();
    for(auto& w : widgets())
    {
        w->set_raw_position({m_container.position().x, m_container.position().y + total_size + m_spacing * index});
        w->set_raw_size({m_container.size().x, size * m_multipliers[index]});
        total_size += size * m_multipliers[index];
        index++;
    }
}

void HorizontalBoxLayout::run()
{
    size_t index = 0, total_size = 0;
    float size = (m_container.size().x - (m_spacing * (widgets().size() - 1))) / widgets().size();
    for(auto& w : widgets())
    {
        w->set_raw_position({m_container.position().x + total_size + m_spacing * index, m_container.position().y});
        w->set_raw_size({size * m_multipliers[index], m_container.size().y});
        total_size += size * m_multipliers[index];
        index++;
    }
}

void BasicLayout::run()
{
    auto resolve_position = [&](double container_size, double widget_size, Length const& expected_position) -> float {
        switch(expected_position.unit())
        {
            case Length::Px: return expected_position.value();
            case Length::PxOtherSide: return container_size - widget_size - expected_position.value();
            default: return 0;
        }
    };

    for(auto& w : widgets())
    {
        auto expected_position = w->expected_position();
        w->set_raw_size({w->expected_size().x.value(), w->expected_size().y.value()});
        auto x = resolve_position(m_container.size().x, w->size().x, expected_position.x);
        auto y = resolve_position(m_container.size().y, w->size().y, expected_position.y);
        w->set_raw_position({x + m_container.position().x, y + m_container.position().y});
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
