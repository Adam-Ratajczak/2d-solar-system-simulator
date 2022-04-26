#pragma once

#include "Container.hpp"
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

    void handle_events();

    sf::Font font;
    sf::Font bold_font;
    sf::Font fixed_width_font;

    enum class NotificationLevel {
        Error
    };
    void spawn_notification(std::string message, NotificationLevel);

private:
    struct Notification {
        int remaining_ticks = 120;
        std::string message;
        NotificationLevel level {};
    };

    void draw_notification(Notification const&, float y) const;

    std::vector<Notification> m_notifications;
};

}
