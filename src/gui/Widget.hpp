#pragma once

#include <SFML/Graphics.hpp>

class Container;

class Widget
{

public:
    Widget(Container* parent)
    : m_parent(parent) {}

    bool is_hover() const { return m_hover; }

    virtual void handle_event(sf::Event& event);
    virtual void draw(sf::RenderWindow& window) const;

    void set_position(sf::Vector2f p)
    {
        m_pos = p;
        set_needs_relayout();
    }
    void set_size(sf::Vector2f s)
    {
        m_size = s;
        set_needs_relayout();
    }

    sf::Vector2f position() const { return m_pos; }
    sf::Vector2f size() const { return m_size; }

    // FIXME: These should be private somehow.
    void relayout_if_needed();
    virtual void update_and_draw(sf::RenderWindow&);

    void set_visible(bool visible) { m_visible = visible; }
    bool is_visible() const { return m_visible; }

protected:
    virtual void relayout() {}
    virtual bool is_mouse_over(sf::Vector2i) const;

    void set_needs_relayout() { m_needs_relayout = true; }

private:
    // FIXME: Full hierarchy, not just GUI -> widgets...
    Container* m_parent = nullptr;
    sf::Vector2f m_pos, m_size;
    bool m_hover = false;
    bool m_needs_relayout = false;
    bool m_visible = true;
};