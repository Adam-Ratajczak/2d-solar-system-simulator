#pragma once

#include <SFML/Graphics.hpp>

class Widget{

public:
    Widget() = default;

    bool is_hover() const { return m_hover; }

    virtual void handle_event(sf::Event& event);
    virtual void draw(sf::RenderWindow& window) const = 0;

    // TODO: Implement proper layout system.
    void set_position(sf::Vector2f p) { m_pos = p; }
    void set_size(sf::Vector2f s) { m_size = s; }

    sf::Vector2f position() const { return m_pos; }
    sf::Vector2f size() const { return m_size; }

protected:
    virtual bool is_mouse_over(sf::Vector2i) const;

private:
    sf::Vector2f m_pos, m_size;
    bool m_hover = false;
};
