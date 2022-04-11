#pragma once

#include "Widget.hpp"

#include <initializer_list>
#include <memory>
#include <type_traits>
#include <vector>

namespace GUI {

class Container;

using WidgetList = std::vector<std::shared_ptr<Widget>>;

class Layout {
public:
    Layout(Container& c)
        : m_container(c) { }

    virtual ~Layout() = default;

    virtual void run() = 0;

    void set_multipliers(std::initializer_list<float> list);

    // Padding = a gap between content (child widgets) and edge
    // TODO: Support separate paddings for sides
    void set_padding(float m) { m_padding = m; }
    float padding() const { return m_padding; }

    std::vector<float> m_multipliers;

protected:
    Container& m_container;
    float m_padding = 0;

    WidgetList& widgets();
};

enum class Orientation {
    Horizontal,
    Vertical
};

/// Widgets are resized to fill up the entire space (in the vertical axis)
class BoxLayout : public Layout {
public:
    BoxLayout(Container& c, Orientation o)
        : Layout(c)
        , m_orientation(o) { }

    // Spacing = a gap between widgets (but not between edges and widgets)
    void set_spacing(float s) { m_spacing = s; }
    virtual void run() override;

    enum class ContentAlignment {
        BoxStart,
        BoxEnd
    };

    void set_content_alignment(ContentAlignment alignment) { m_content_alignment = alignment; }

private:
    Orientation m_orientation;
    ContentAlignment m_content_alignment = ContentAlignment::BoxStart;
    float m_spacing = 0;
};

class VerticalBoxLayout : public BoxLayout {
public:
    VerticalBoxLayout(Container& c)
        : BoxLayout(c, Orientation::Vertical) { }
};

class HorizontalBoxLayout : public BoxLayout {
public:
    HorizontalBoxLayout(Container& c)
        : BoxLayout(c, Orientation::Horizontal) { }
};

// Just assigns input_size to size.
class BasicLayout : public Layout {
public:
    BasicLayout(Container& c)
        : Layout(c) { }

private:
    virtual void run() override;
};

class Container : public Widget {
public:
    explicit Container(Container& parent)
        : Widget(parent) { }

    template<class T, class... Args>
    requires(std::is_base_of_v<Widget, T>&& requires(Container& c, Args&&... args) { T(c, args...); })
        std::shared_ptr<T> add_widget(Args&&... args) {
        auto widget = std::make_shared<T>(*this, std::forward<Args>(args)...);
        m_widgets.push_back(widget);
        if (m_layout)
            m_layout->m_multipliers.push_back(1);
        set_needs_relayout();
        return widget;
    }

    void add_created_widget(std::shared_ptr<Widget> widget) {
        m_widgets.push_back(std::move(widget));
        if (m_layout)
            m_layout->m_multipliers.push_back(1);
        set_needs_relayout();
    }

    virtual void do_update() override;
    virtual void do_handle_event(Event&) override;
    virtual void do_relayout() override;
    virtual void do_draw(sf::RenderWindow& window) const override;

    template<class T, class... Args>
    requires(std::is_base_of_v<Layout, T>&& requires(Container& c, Args&&... args) { T(c, args...); })
        T& set_layout(Args&&... args) {
        auto layout = std::make_unique<T>(*this, std::forward<Args>(args)...);
        auto layout_ptr = layout.get();
        m_layout = std::move(layout);
        return *layout_ptr;
    }

    void clear_layout() { m_layout = nullptr; }
    std::unique_ptr<Layout>& get_layout() { return m_layout; }

    virtual void dump(unsigned depth) override;

protected:
    explicit Container(Application& application)
        : Widget(application) { }

    virtual void relayout() override;
    WidgetList m_widgets;
    Widget* m_focused_widget { nullptr };

    virtual float intrinsic_padding() const { return 0; }

private:
    friend Layout;

    std::unique_ptr<Layout> m_layout;
};

}
