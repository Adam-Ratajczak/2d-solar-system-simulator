#include "Application.hpp"

#include "../gfx/RoundedEdgeRectangleShape.hpp"
#include "WidgetTreeRoot.hpp"

#include <cassert>
#include <iostream>

namespace GUI {

Application* s_the {};

Application& Application::the() {
    assert(s_the);
    return *s_the;
}

Application::Application(sf::RenderWindow& wnd)
    : WidgetTreeRoot(wnd) {
    assert(!s_the);
    s_the = this;
    font.loadFromFile("../assets/fonts/Xolonium-pn4D.ttf");
    bold_font.loadFromFile("../assets/fonts/XoloniumBold-xKZO.ttf");
    fixed_width_font.loadFromFile("../assets/fonts/SourceCodePro-Regular.otf");
}

void Application::handle_events() {
    sf::Event event;
    while (window().pollEvent(event)) {
        Event gui_event(event);
        handle_event(gui_event);
    }
}

void Application::draw_notification(Notification const& notification, float y) const {
    sf::Text text { notification.message, font, 15 };
    auto text_bounds = text.getLocalBounds();
    text.setPosition(window().getSize().x - text_bounds.width - 20, y + 20);

    RoundedEdgeRectangleShape rs { { text_bounds.width + 20, text_bounds.height + 30 }, 10 };
    rs.setPosition(text_bounds.left - 10 + text.getPosition().x, text_bounds.top - 15 + text.getPosition().y);
    sf::Color rs_fill_color;
    switch (notification.level) {
    case NotificationLevel::Error:
        rs_fill_color = { 155, 50, 50, 100 };
    }
    rs.setFillColor(rs_fill_color);
    window().draw(rs);
    window().draw(text);
}

void Application::spawn_notification(std::string message, NotificationLevel level) {
    m_notifications.push_back(Notification { .message = std::move(message), .level = level });
}

}
