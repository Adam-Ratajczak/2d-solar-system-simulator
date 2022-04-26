#include "Application.hpp"

#include "../gfx/ClipViewScope.hpp"
#include "../gfx/RoundedEdgeRectangleShape.hpp"
#include "WidgetTreeRoot.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
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

sf::Event Application::transform_event(sf::Vector2f offset, sf::Event event) const {
    if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
        event.mouseButton.x -= offset.x;
        event.mouseButton.y -= offset.y;
    }
    else if (event.type == sf::Event::MouseMoved) {
        event.mouseMove.x -= offset.x;
        event.mouseMove.y -= offset.y;
    }

    return event;
}

void Application::handle_events() {
    sf::Event event;
    while (window().pollEvent(event)) {
        if (event.type == sf::Event::MouseButtonPressed) {
            m_focused_tool_window = nullptr;
            sf::Vector2f position { static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y) };
            for (auto it = m_tool_windows.rbegin(); it != m_tool_windows.rend(); it++) {
                auto& tool_window = **it;
                if (tool_window.full_rect().contains(position)) {
                    m_focused_tool_window = &tool_window;
                    break;
                }
            }
        }

        if (m_focused_tool_window) {
            Event gui_event_relative_to_tool_window { transform_event(m_focused_tool_window->position(), event) };
            m_focused_tool_window->handle_event(gui_event_relative_to_tool_window);
            if (event.type != sf::Event::Closed)
                continue;
        }
        for (auto it = m_tool_windows.rbegin(); it != m_tool_windows.rend(); it++) {
            auto& tool_window = **it;
            Event gui_event_relative_to_tool_window { transform_event(tool_window.position(), event) };
            if (event.type == sf::Event::MouseMoved) {
                tool_window.handle_event(gui_event_relative_to_tool_window);
                break;
            }
        }

        Event gui_event(event);
        handle_event(gui_event);
    }
}

void Application::draw() {
    WidgetTreeRoot::draw();
    for (auto& tool_window : m_tool_windows) {
        auto color = tool_window.get() == m_focused_tool_window ? sf::Color(160, 160, 160, 100) : sf::Color(127, 127, 127, 100);

        RoundedEdgeRectangleShape rs_titlebar({ tool_window->size().x + 2, ToolWindow::TitleBarSize }, 5);
        rs_titlebar.set_border_radius_bottom_left(0);
        rs_titlebar.set_border_radius_bottom_right(0);
        rs_titlebar.setPosition(tool_window->position() - sf::Vector2f(1, ToolWindow::TitleBarSize));
        rs_titlebar.setFillColor(color);
        window().draw(rs_titlebar);

        sf::Text text(tool_window->title(), font, 15);
        text.setPosition(tool_window->position() + sf::Vector2f(10, 4 - ToolWindow::TitleBarSize));
        window().draw(text);

        sf::RectangleShape rs_border(tool_window->size() - sf::Vector2f(0, 1));
        rs_border.setPosition(tool_window->position() + sf::Vector2f(0, 1));
        rs_border.setFillColor(sf::Color::Transparent);
        rs_border.setOutlineColor(color);
        rs_border.setOutlineThickness(1);
        window().draw(rs_border);
        {
            Gfx::ClipViewScope scope(window(), tool_window->rect(), Gfx::ClipViewScope::Mode::Override);
            tool_window->draw();
        }
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

ToolWindow& Application::open_tool_window(sf::String title) {
    auto tool_window = std::make_unique<ToolWindow>(window());
    auto tool_window_ptr = tool_window.get();
    tool_window_ptr->set_title(std::move(title));
    m_tool_windows.push_back(std::move(tool_window));
    return *tool_window_ptr;
}

}
