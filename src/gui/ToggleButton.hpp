#pragma once

#include "Button.hpp"
#include "NotifyUser.hpp"

class ToggleButton : public Button {
public:
    ToggleButton(Container&, sf::Image);

    bool is_active() const { return m_active; }

    void set_active(bool active, NotifyUser notify_user = NotifyUser::Yes) {
        m_active = active;
        if (notify_user == NotifyUser::Yes && on_change)
            on_change(active);
    }

    std::function<void(bool)> on_change;

private:
    virtual void on_click_impl();
    virtual sf::Color color_for_state() const;

    bool m_active { false };
};
