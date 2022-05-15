#include "Application.hpp"

#include "../gfx/ClipViewScope.hpp"
#include "../gfx/RoundedEdgeRectangleShape.hpp"
#include "ToolWindow.hpp"
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

void Application::focus_window(WindowList::iterator new_focused_it) {
    if (new_focused_it == m_tool_windows.end())
        return;
    m_focused_tool_window = new_focused_it->get();
    auto ptr = std::move(*new_focused_it);
    m_tool_windows.erase(new_focused_it);
    m_tool_windows.push_back(std::move(ptr));
}

void Application::handle_event(sf::Event event) {
    if (event.type == sf::Event::Closed) {
        // TODO: Allow user to override it.
        quit();
        return;
    }

    // Focus window if mouse button pressed
    if (event.type == sf::Event::MouseButtonPressed) {
        m_focused_tool_window = nullptr;
        sf::Vector2f position { static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y) };
        decltype(m_tool_windows)::iterator new_focused_it = m_tool_windows.end();
        for (auto it = m_tool_windows.end(); it != m_tool_windows.begin();) {
            auto it2 = --it;
            auto& tool_window = **it2;
            if (tool_window.full_rect().contains(position)) {
                new_focused_it = it2;
                break;
            }
        }
        focus_window(new_focused_it);
    }

    // Pass events to focused tool window
    if (m_focused_tool_window) {
        m_focused_tool_window->handle_event(transform_event(m_focused_tool_window->position(), event));
        bool scroll_outside_window = event.type == sf::Event::MouseWheelScrolled
            && !m_focused_tool_window->full_rect().contains({ static_cast<float>(event.mouseWheelScroll.x), static_cast<float>(event.mouseWheelScroll.y) });
        if (!(event.type == sf::Event::Closed || event.type == sf::Event::MouseMoved || event.type == sf::Event::MouseButtonReleased || scroll_outside_window))
            return;
    }

    bool should_pass_event_to_main_window = true;

    // Pass mouse moves to all tool windows + capture all scrolls
    for (auto it = m_tool_windows.rbegin(); it != m_tool_windows.rend(); it++) {
        auto& tool_window = **it;
        if (event.type == sf::Event::MouseMoved) {
            tool_window.handle_event(transform_event(tool_window.position(), event));
            break;
        }

        bool scroll_on_window = event.type == sf::Event::MouseWheelScrolled
            && tool_window.full_rect().contains({ static_cast<float>(event.mouseWheelScroll.x), static_cast<float>(event.mouseWheelScroll.y) });

        if (scroll_on_window)
            should_pass_event_to_main_window = false;
    }

    if (should_pass_event_to_main_window)
        WidgetTreeRoot::handle_event(event);
}

void Application::handle_events() {
    sf::Event event;
    while (window().pollEvent(event))
        handle_event(event);
}

void Application::draw() {
    WidgetTreeRoot::draw();
    for (auto& tool_window : m_tool_windows)
        tool_window->draw();
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

ToolWindow& Application::open_tool_window_impl(std::unique_ptr<ToolWindow> tool_window) {
    auto tool_window_ptr = tool_window.get();
    tool_window_ptr->set_position(m_next_tool_window_position);
    m_next_tool_window_position += sf::Vector2f(ToolWindow::TitleBarSize * 2, ToolWindow::TitleBarSize * 2);
    if (m_next_tool_window_position.x > size().x - ToolWindow::TitleBarSize || m_next_tool_window_position.y > size().y - ToolWindow::TitleBarSize)
        m_next_tool_window_position = { 10, 10 };
    m_tool_windows.push_back(std::move(tool_window));
    return *tool_window_ptr;
}

Application::OpenOrFocusResult Application::open_or_focus_tool_window(sf::String title, std::string id) {
    for (auto it = m_tool_windows.begin(); it != m_tool_windows.end(); it++) {
        auto window = it->get();
        if (window->id() == id) {
            focus_window(it);
            return { .window = window, .opened = false };
        }
    }
    OpenOrFocusResult result = { .window = &open_tool_window(std::move(id)), .opened = true };
    result.window->set_title(std::move(title));
    return result;
}

void Application::update() {
    WidgetTreeRoot::update();
    std::erase_if(m_tool_windows, [&](auto& wnd) {
        if (wnd->is_closed()) {
            if (wnd->on_close)
                wnd->on_close();
            if (wnd.get() == m_focused_tool_window)
                m_focused_tool_window = nullptr;
            return true;
        }
        return false;
    });
    for (auto& tool_window : m_tool_windows)
        tool_window->update();
}

}
