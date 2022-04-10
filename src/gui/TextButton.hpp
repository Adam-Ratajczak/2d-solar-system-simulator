#pragma once
#include "Button.hpp"
#include "Widget.hpp"
#include "NotifyUser.hpp"
#include <SFML/Graphics.hpp>

class TextButton : public Widget {
public:
    enum class Align {
        CenterLeft,
        Center,
        CenterRight
    };
    explicit TextButton(Container& c)
        : Widget(c) { }

    void set_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color);
    void set_active_display_attributes(sf::Color bg_color, sf::Color fg_color, sf::Color text_color);
    virtual void draw(sf::RenderWindow& window) const override;
    virtual void handle_event(Event&) override;

    void set_content(std::string content) { m_content = content; }
    void set_active_content(std::string content) { m_active_content = content; }

    unsigned get_font_size() const { return m_font_size; }
    void set_font_size(unsigned font_size) { m_font_size = font_size; }

    Align get_alignment() const { return m_alignment; };
    void set_alignment(Align alignment) { m_alignment = alignment; };

    bool is_active() const { return m_active; }

    void set_active(bool active, NotifyUser notify_user = NotifyUser::Yes) {
        if (m_active != active) {
            m_active = active;
            if (notify_user == NotifyUser::Yes && on_change)
                on_change(active);
        }
    }

    std::function<void(bool)> on_change;

private:
    std::string m_content, m_active_content;
    sf::Color m_bg_color = sf::Color(0, 0, 0), m_fg_color = sf::Color(0, 0, 255), m_text_color = sf::Color(255, 255, 255);
    sf::Color m_active_bg_color = sf::Color(0, 0, 0), m_active_fg_color = sf::Color(0, 0, 255), m_active_text_color = sf::Color(255, 255, 255);
    unsigned m_font_size = 20;
    Align m_alignment = Align::CenterLeft;

    bool m_active { false };
};
