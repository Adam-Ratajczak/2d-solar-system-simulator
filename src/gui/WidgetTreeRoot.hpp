#pragma once

#include "Tooltip.hpp"
#include "Widget.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <list>
#include <memory>

namespace GUI {

class WidgetTreeRoot {
public:
    explicit WidgetTreeRoot(sf::RenderWindow& wnd)
        : m_window(wnd) { }

    virtual ~WidgetTreeRoot() = default;

    sf::RenderWindow& window() const { return m_window; }
    Widget* focused_widget() const { return m_focused_widget; }
    void set_focused_widget(Widget* w);

    void set_needs_relayout() { m_needs_relayout = true; }

    template<class T, class... Args>
    auto& set_main_widget(Args&&... args) {
        auto widget = std::make_shared<T>(*this, std::forward<Args>(args)...);
        auto widget_ptr = widget.get();
        m_main_widget = std::move(widget);
        m_needs_relayout = true;
        return *widget_ptr;
    }

    Tooltip& add_tooltip(std::unique_ptr<Tooltip> t) {
        // std::cout << t->owner << " ADDED TOOLTIP" << std::endl;
        auto t_ptr = t.get();
        m_tooltips.push_back(std::move(t));
        return *t_ptr;
    }
    void remove_tooltip(Tooltip* t);

    virtual void draw();
    void handle_event(Event&);
    virtual void update() { m_main_widget->do_update(); }

    virtual sf::Vector2f position() const = 0;
    virtual sf::Vector2f size() const = 0;
    sf::FloatRect rect() const { return { position(), size() }; }

protected:
private:
    sf::RenderWindow& m_window;
    Widget* m_focused_widget {};
    bool m_needs_relayout = true;
    std::shared_ptr<Widget> m_main_widget;
    std::list<std::unique_ptr<Tooltip>> m_tooltips;
};

}