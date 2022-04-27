#include "WidgetTreeRoot.hpp"

#include "Application.hpp"

#include <cassert>
#include <iostream>

namespace GUI {

void WidgetTreeRoot::set_focused_widget(Widget* w) {
    m_focused_widget = w;
}

void WidgetTreeRoot::draw() {
    sf::View gui_view { sf::FloatRect(position(), size()) };
    m_window.setView(gui_view);

    if (m_needs_relayout) {
        m_main_widget->set_size({ { size().x, Length::Unit::Px }, { size().y, Length::Unit::Px } });
        m_main_widget->set_raw_size(size());
        m_main_widget->do_relayout();
        //m_main_widget->dump(0);
        m_needs_relayout = false;
    }
    m_main_widget->do_draw(m_window);

    m_window.setView(gui_view);
    for (auto& tooltip : m_tooltips) {
        sf::Text text(tooltip->text, Application::the().font, 15);
        text.setFillColor(sf::Color::Black);
        text.setPosition(tooltip->position);

        auto bounds = text.getGlobalBounds();

        sf::RectangleShape bg { { bounds.width + 10, bounds.height + 10 } };
        auto x_pos = std::min(m_window.getSize().x - bg.getSize().x, bounds.left - 5);
        bg.setPosition(x_pos, bounds.top - 5);
        text.setPosition(x_pos + 5, text.getPosition().y);
        m_window.draw(bg);

        m_window.draw(text);
    }
}

void WidgetTreeRoot::remove_tooltip(Tooltip* t) {
    std::erase_if(m_tooltips, [&](auto& other_t) { return other_t.get() == t; });
}

void WidgetTreeRoot::handle_event(Event& event) {
    m_main_widget->do_handle_event(event);
}

}
