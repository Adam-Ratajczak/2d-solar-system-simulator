#pragma once

#include "Container.hpp"
#include "Tooltip.hpp"

#include <SFML/Graphics/View.hpp>
#include <iostream>
#include <list>

namespace GUI {

class Application {
public:
    explicit Application(sf::RenderWindow&);

    static Application& the();

    sf::RenderWindow& window() const { return m_window; }
    Widget* focused_widget() const { return m_focused_widget; }
    void set_focused_widget(Widget* w);

    void set_needs_relayout() { m_needs_relayout = true; }

    template<class T, class... Args>
    auto& set_main_widget(Args&&... args)
    {
        auto widget = std::make_shared<T>(*this, std::forward<Args>(args)...);
        auto widget_ptr = widget.get();
        m_main_widget = std::move(widget);
        return *widget_ptr;
    }

    Tooltip& add_tooltip(std::unique_ptr<Tooltip> t) {
        //std::cout << t->owner << " ADDED TOOLTIP" << std::endl;
        auto t_ptr = t.get();
        m_tooltips.push_back(std::move(t));
        return *t_ptr;
    }
    void remove_tooltip(Tooltip* t);

    void handle_events();
    void draw();

    sf::Font font;
    sf::Font bold_font;
    sf::Font fixed_width_font;

    void update(){m_main_widget->do_update();}
    void set_view(sf::View view){m_window.setView(view);}

private:
    sf::RenderWindow& m_window;
    Widget* m_focused_widget {};
    bool m_needs_relayout = true;
    std::shared_ptr<Widget> m_main_widget;
    std::list<std::unique_ptr<Tooltip>> m_tooltips;
};

}
