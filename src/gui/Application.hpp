#pragma once

#include "Container.hpp"
#include "ToolWindow.hpp"
#include "Tooltip.hpp"
#include "WidgetTreeRoot.hpp"

#include <SFML/Graphics/View.hpp>
#include <iostream>
#include <list>

namespace GUI {

class Application : public WidgetTreeRoot {
public:
    explicit Application(sf::RenderWindow&);

    static Application& the();

    virtual void draw() override;
    void handle_events();
    virtual void update() override;

    sf::Font font;
    sf::Font bold_font;
    sf::Font fixed_width_font;

    enum class NotificationLevel {
        Error
    };
    void spawn_notification(std::string message, NotificationLevel);

    ToolWindow& open_tool_window(sf::String title, std::string id = "ToolWindow");
    struct OpenOrFocusResult {
        ToolWindow* window {};
        bool opened {};
    };
    OpenOrFocusResult open_or_focus_tool_window(sf::String title, std::string id);
    ToolWindow* focused_tool_window() const { return m_focused_tool_window; }

    virtual sf::Vector2f position() const override { return {}; }
    virtual sf::Vector2f size() const override { return sf::Vector2f { window().getSize() }; }

private:
    struct Notification {
        int remaining_ticks = 120;
        std::string message;
        NotificationLevel level {};
    };

    void draw_notification(Notification const&, float y) const;
    sf::Event transform_event(sf::Vector2f offset, sf::Event event) const;

    using WindowList = std::list<std::unique_ptr<ToolWindow>>;

    void focus_window(WindowList::iterator);

    WindowList m_tool_windows;
    sf::Vector2f m_next_tool_window_position { 10, 10 + ToolWindow::TitleBarSize };
    ToolWindow* m_focused_tool_window = nullptr;
    std::vector<Notification> m_notifications;
};

}
