#include "Container.hpp"

#include <iostream>

WidgetList& Layout::widgets() {
    return m_container.m_widgets;
}

void Layout::set_multipliers(std::initializer_list<float> list) {
    m_multipliers.clear();

    for (auto& l : list) {
        m_multipliers.push_back(l);
    }
}

void BoxLayout::run() {
    // std::cout << "BOXLAYOUT " << m_container.size().x << "," << m_container.size().y << " spacing=" << m_spacing << std::endl;
    auto vec2f_main_coord_by_orientation = [this](auto vec) -> auto{
        if (m_orientation == Orientation::Horizontal)
            return vec.x;
        return vec.y;
    };
    auto vec2f_cross_coord_by_orientation = [this](auto vec) -> auto{
        if (m_orientation == Orientation::Vertical)
            return vec.x;
        return vec.y;
    };
    auto convert_vector_by_orientation = [this](sf::Vector2f vec) {
        if (m_orientation == Orientation::Vertical)
            return sf::Vector2f { vec.y, vec.x };
        return vec;
    };

    // 1. Compute widget size (in main axis) if it has fixed size
    for (auto& w : widgets()) {
        if (!w->is_visible())
            continue;

        float size = 0;
        switch (vec2f_main_coord_by_orientation(w->input_size()).unit()) {
        case Length::Px:
        case Length::PxOtherSide:
            // std::cout << "test" << std::endl;
            size = vec2f_main_coord_by_orientation(w->input_size()).value();
            break;
        case Length::Percent:
            // std::cout << size << std::endl;
            size = vec2f_main_coord_by_orientation(w->input_size()).value() * vec2f_main_coord_by_orientation(m_container.size()) / 100.0;
            break;
        case Length::Auto:
            size = 0;
            break;
        }
        w->set_raw_size(convert_vector_by_orientation({ size, vec2f_cross_coord_by_orientation(m_container.size()) - m_padding * 2 }));
    }

    // 2. Compute size available for auto-sized widgets
    float available_size_for_autosized_widgets = vec2f_main_coord_by_orientation(m_container.size()) - m_padding * 2;
    size_t autosized_widget_count = 0;
    for (auto& w : widgets()) {
        if (!w->is_visible())
            continue;
        if (vec2f_main_coord_by_orientation(w->input_size()).unit() == Length::Auto)
            autosized_widget_count++;
        else
            available_size_for_autosized_widgets -= vec2f_main_coord_by_orientation(w->size()) + m_spacing;
    }

    // 3. Set autosized widgets' size + arrange widgets
    float autosized_widget_size = (available_size_for_autosized_widgets - (m_spacing * (autosized_widget_count - 1))) / autosized_widget_count;
    size_t index = 0;
    switch (m_content_alignment) {
    case ContentAlignment::BoxStart: {
        float current_position = 0;
        for (auto& w : widgets()) {
            if (!w->is_visible())
                continue;
            if (vec2f_main_coord_by_orientation(w->input_size()).unit() == Length::Auto)
                w->set_raw_size(convert_vector_by_orientation({ autosized_widget_size, vec2f_cross_coord_by_orientation(m_container.size()) - m_padding * 2 }));
            w->set_raw_position(convert_vector_by_orientation({ vec2f_main_coord_by_orientation(m_container.position()) + current_position + m_padding,
                vec2f_cross_coord_by_orientation(m_container.position()) + m_padding }));
            current_position += vec2f_main_coord_by_orientation(w->size()) + m_spacing;
            index++;
        }
    } break;
    case ContentAlignment::BoxEnd: {
        float current_position = vec2f_main_coord_by_orientation(m_container.size());
        for (auto it = widgets().rbegin(); it != widgets().rend(); it++) {
            auto& w = *it;
            if (!w->is_visible())
                continue;
            if (vec2f_main_coord_by_orientation(w->input_size()).unit() == Length::Auto)
                w->set_raw_size(convert_vector_by_orientation({ autosized_widget_size, vec2f_cross_coord_by_orientation(m_container.size()) - m_padding * 2 }));
            current_position -= vec2f_main_coord_by_orientation(w->size()) + m_spacing;
            w->set_raw_position(convert_vector_by_orientation({ vec2f_main_coord_by_orientation(m_container.position()) + current_position + m_padding,
                vec2f_cross_coord_by_orientation(m_container.position()) + m_padding }));
            index++;
        }
    } break;
    }

    /*for(auto& w : widgets())
    {
        std::cout << w->input_size().x << "," << w->input_size().y << " @ " << w->input_position().x << "," << w->input_position().y << " ----> ";
        std::cout << w->size().x << "," << w->size().y << " @ " << w->position().x << "," << w->position().y << std::endl;
    }*/
}

void BasicLayout::run() {
    auto resolve_position = [&](double container_size, double widget_size, Length const& input_position) -> float {
        switch (input_position.unit()) {
        case Length::Px:
            return input_position.value();
        case Length::PxOtherSide:
            return container_size - widget_size - input_position.value();
        case Length::Percent:
            return widget_size * container_size / 100.0;
        default:
            return 0;
        }
    };

    auto resolve_size = [&](double container_size, Length const& input_size) -> float {
        switch (input_size.unit()) {
        case Length::Px:
        case Length::PxOtherSide:
            return input_size.value();
        case Length::Percent:
            return input_size.value() * container_size / 100.0;
        default:
            return 0;
        }
    };

    for (auto& w : widgets()) {
        auto input_position = w->input_position();
        w->set_raw_size({ resolve_size(m_container.size().x, w->input_size().x), resolve_size(m_container.size().y, w->input_size().y) });
        auto x = resolve_position(m_container.size().x, w->size().x, input_position.x);
        auto y = resolve_position(m_container.size().y, w->size().y, input_position.y);
        w->set_raw_position({ x + m_container.position().x, y + m_container.position().y });
    }
}

void Container::do_relayout() {
    Widget::do_relayout();
    for (auto const& w : m_widgets) {
        if (w->is_visible())
            w->do_relayout();
    }
}

void Container::do_draw(sf::RenderWindow& window) const {
    Widget::do_draw(window);
    for (auto const& w : m_widgets) {
        if (w->is_visible())
            w->do_draw(window);
    }
}

void Container::do_handle_event(Event& event) {
    for (auto it = m_widgets.rbegin(); it != m_widgets.rend(); it++) {
        auto& widget = *it;
        if (!widget->is_visible() || !widget->is_enabled())
            continue;

        widget->do_handle_event(event);
        if (event.is_handled())
            break;
    }
    // FIXME: Proper stacking contexts
    if (!event.is_handled())
        Widget::do_handle_event(event);
}

void Container::do_update() {
    Widget::do_update();
    for (auto const& w : m_widgets)
        w->do_update();
}

void Container::relayout() {
    // std::cout << "TEST " << typeid(*this).name() << std::endl;

    for (auto& w : m_widgets) {
        if (w->m_input_size == LengthVector {}) {
            w->m_input_size = w->initial_size();
            // std::cout << this << " " << typeid(*this).name() << " set initial size" << std::endl;
        }
    }
    if (!m_layout)
        return;
    if (m_layout->padding() == 0)
        m_layout->set_padding(intrinsic_padding());
    m_layout->run();
}

void Container::dump(unsigned depth) {
    Widget::dump(depth);
    ++depth;
    for (auto& w : m_widgets)
        w->dump(depth);
}
