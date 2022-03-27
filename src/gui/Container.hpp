#pragma once

#include "Widget.hpp"

#include <memory>
#include <type_traits>

class Container;

using WidgetList = std::vector<std::shared_ptr<Widget>>;

class Layout
{
public:
    Layout(Container& c)
    : m_container(c) {}

    virtual void run() = 0;

protected:
    Container& m_container;

    WidgetList& widgets();
};

/// Widgets are resized to fill up the entire space (in the vertical axis)
class VerticalBoxLayout : public Layout
{
public:
    VerticalBoxLayout(Container& c)
    : Layout(c) {}

    void set_spacing(float s) { m_spacing = s; }

private:
    virtual void run() override;

    float m_spacing;
};

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

    enum class LayoutMode
    {
        None,           // No layouting is done at all.
        VerticalBox,    
    };

    template<class T, class... Args> requires(std::is_base_of_v<Layout, T> && requires(Container& c, Args&&... args) { T(c, args...); })
    T& set_layout(Args&&... args)
    {
        auto layout = std::make_unique<T>(*this, std::forward<Args>(args)...);
        auto layout_ptr = layout.get();
        m_layout = std::move(layout);
        return *layout_ptr;
    }

    void clear_layout() { m_layout = nullptr; }

protected:
    virtual void relayout() override;

private:
    friend Layout;

    std::unique_ptr<Layout> m_layout;
    WidgetList m_widgets;
};
