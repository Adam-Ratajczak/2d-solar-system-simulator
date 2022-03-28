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
    // 1. Compute widget size (in main axis) if it has fixed size
    for(auto& w : widgets())
    {
        float size = 0;
        switch(w->input_size().x.unit())
        {
            case Length::Px:
            case Length::PxOtherSide:
                // std::cout << "test" << std::endl;
                size = w->input_size().x.value();
                break;
            case Length::Auto:
                size = 0;
                break;
        }
        w->set_raw_size({size, m_container.size().y});
    }

    // 2. Compute size available for auto-sized widgets
    float available_size_for_autosized_widgets = m_container.size().x;
    size_t autosized_widget_count = 0;
    for(auto& w : widgets())
    {
        if(w->input_size().x.unit() == Length::Auto)
            autosized_widget_count++;
        else
            available_size_for_autosized_widgets -= w->size().x + m_spacing;
    }

    // 3. Set autosized widgets' size + arrange widgets
    float autosized_widget_size = (available_size_for_autosized_widgets - (m_spacing * (autosized_widget_count - 1))) / autosized_widget_count;
    float current_position = 0;
    size_t index = 0;
    for(auto& w : widgets())
    {
        if(w->input_size().x.unit() == Length::Auto)
            w->set_raw_size({autosized_widget_size, m_container.size().y});
        w->set_raw_position({m_container.position().x + current_position, m_container.position().y});
        current_position += w->size().x + m_spacing;
        index++;
    }
}

void BasicLayout::run()
{
    auto resolve_position = [&](double container_size, double widget_size, Length const& input_position) -> float {
        switch(input_position.unit())
        {
            case Length::Px: return input_position.value();
            case Length::PxOtherSide: return container_size - widget_size - input_position.value();
            default: return 0;
        }
    };

    for(auto& w : widgets())
    {
        auto input_position = w->input_position();
        w->set_raw_size({w->input_size().x.value(), w->input_size().y.value()});
        auto x = resolve_position(m_container.size().x, w->size().x, input_position.x);
        auto y = resolve_position(m_container.size().y, w->size().y, input_position.y);
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
