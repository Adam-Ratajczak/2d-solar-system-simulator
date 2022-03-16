#pragma once

#include "Constants.hpp"
#include "Vector2.hpp"

#include <SFML/Graphics.hpp>

class View
{
public:
    explicit View(sf::RenderWindow& target)
    : m_target(target) {}

    static constexpr double SCALE = 1000 / AU;

    void set_offset(Vector2 o) { m_offset = o; }
    void set_zoom(double d) { m_zoom = d; }
    Vector2 offset() const { return m_offset; }
    double scale() const { return m_zoom * SCALE; }
    void apply_zoom(double v) { m_zoom *= v; }
    sf::RenderWindow& target() const { return m_target; } 

    Vector2 world_to_screen(Vector2 v) const { return (v - m_offset) * scale() + Vector2(m_target.getSize() / 2u); }
    Vector2 screen_to_world(Vector2 v) const { return (v - Vector2(m_target.getSize() / 2u)) / scale() + m_offset; }

    void reset() { m_offset = Vector2{}; m_zoom = 1; };
private:
    sf::RenderWindow& m_target;
    Vector2 m_offset;
    double m_zoom = 1;
};
