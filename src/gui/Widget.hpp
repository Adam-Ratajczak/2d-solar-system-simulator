#pragma once

#include "Units.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class Container;
class Application;

struct LengthVector
{
    Length x;
    Length y;
};

class Event
{
public:
    Event(sf::Event const& event)
    : m_event(event) {}

    sf::Event event() const { return m_event; }
    bool is_handled() const { return m_handled; }
    void set_handled() { m_handled = true; }

    sf::Event::EventType type() const { return m_event.type; }

private:
    sf::Event m_event;
    bool m_handled = false;
};

class Widget
{
public:
    explicit Widget(Container& parent);

    virtual ~Widget();

    bool is_hover() const { return m_hover; }

    virtual void do_handle_event(Event& event);
    virtual void draw(sf::RenderWindow& window) const;

    void set_raw_position(sf::Vector2f p)
    {
        m_pos = p;
    }
    void set_raw_size(sf::Vector2f s)
    {
        m_size = s;
    }

    void set_position(LengthVector l)
    {
        m_expected_pos = l;
        set_needs_relayout();
    }

    void set_size(LengthVector l)
    {
        m_input_size = l;
        set_needs_relayout();
    }

    sf::Vector2f position() const { return m_pos; }
    sf::Vector2f size() const { return m_size; }
    LengthVector input_position() const { return m_expected_pos; }
    LengthVector input_size() const { return m_input_size; }

    // FIXME: These should be private somehow.
    void relayout_if_needed();
    virtual void update_and_draw(sf::RenderWindow&);

    void set_visible(bool visible)
    {
        m_visible = visible;
        set_needs_relayout();
    }

    bool is_visible() const { return m_visible; }

    void set_focused();
    bool is_focused() const;

    sf::RenderWindow& window() const;
    Container* parent() const { return m_parent; }

protected:
    explicit Widget(Application& application)
    : m_application(application) {}

    virtual void relayout() {}
    virtual bool is_mouse_over(sf::Vector2i) const;
    virtual void update() {}
    virtual void handle_event(Event&);

    void set_needs_relayout() { m_needs_relayout = true; }

private:
    friend Container;

    Container* m_parent = nullptr;
    Application& m_application;
    sf::Vector2f m_pos, m_size;
    LengthVector m_expected_pos, m_input_size;
    bool m_hover = false;
    bool m_needs_relayout = true;
    bool m_visible = true;
};
