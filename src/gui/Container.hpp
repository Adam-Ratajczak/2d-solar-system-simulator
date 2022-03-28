#pragma once

#include "Widget.hpp"

#include <initializer_list>
#include <memory>
#include <type_traits>
#include <vector>

class Container;

using WidgetList = std::vector<std::shared_ptr<Widget>>;

class Layout
{
public:
    Layout(Container& c)
    : m_container(c) {}

    virtual void run() = 0;

    void set_multipliers(std::initializer_list<float> list);

    std::vector<float> m_multipliers;
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

class HorizontalBoxLayout : public Layout
{
public:
    HorizontalBoxLayout(Container& c)
    : Layout(c) {}

    void set_spacing(float s) { m_spacing = s; }

private:
    virtual void run() override;

    float m_spacing;
};

// Just assigns input_size to size.
class BasicLayout : public Layout
{
public:
    BasicLayout(Container& c)
    : Layout(c) {}

private:
    virtual void run() override;
};

class Container : public Widget
{
public:
    explicit Container(sf::RenderWindow& wnd)
    : Widget(wnd) {}

    explicit Container(Container& parent)
    : Widget(parent) {}

    template<class T, class... Args> requires(std::is_base_of_v<Widget, T> && requires(Container& c, Args&&... args) { T(c, args...); })
    std::shared_ptr<T> add_widget(Args&&... args)
    {
        auto widget = std::make_shared<T>(*this, std::forward<Args>(args)...);
        m_widgets.push_back(widget);
        m_layout->m_multipliers.push_back(1);
        return widget;
    }

    virtual void handle_event(sf::Event& event) override;
    virtual void update_and_draw(sf::RenderWindow& window) override;

    template<class T, class... Args> requires(std::is_base_of_v<Layout, T> && requires(Container& c, Args&&... args) { T(c, args...); })
    T& set_layout(Args&&... args)
    {
        auto layout = std::make_unique<T>(*this, std::forward<Args>(args)...);
        auto layout_ptr = layout.get();
        m_layout = std::move(layout);
        return *layout_ptr;
    }

    void clear_layout() { m_layout = nullptr; }
    std::unique_ptr<Layout>& get_layout() {return m_layout;}

protected:
    virtual void relayout() override;

private:
    friend Layout;

    std::unique_ptr<Layout> m_layout;
    WidgetList m_widgets;
};
