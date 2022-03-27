#pragma once

#include "Widget.hpp"

#include <memory>
#include <type_traits>

class Container : public Widget
{
public:
    Container(Container* parent)
    : Widget(parent) {}

    template<class T, class... Args> requires(std::is_base_of_v<Widget, T> && requires(Container* c, Args&&... args) { T(c, args...); })
    std::shared_ptr<T> add_widget(Args&&... args)
    {
        auto widget = std::make_shared<T>(this, std::forward<Args>(args)...);
        m_widgets.push_back(widget);
        return widget;
    }

    virtual void handle_event(sf::Event& event) override;
    virtual void update_and_draw(sf::RenderWindow& window) override;

private:
    std::vector<std::shared_ptr<Widget>> m_widgets;
};
