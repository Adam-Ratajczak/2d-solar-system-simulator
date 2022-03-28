#pragma once

#include "../Constants.hpp"
#include "../Vector2.hpp"
#include "Widget.hpp"

class Object;
class World;

class SimulationView : public Widget
{
public:
    explicit SimulationView(Container& c, World& world)
    : Widget(c), m_world(world) {}

    static constexpr double SCALE = 1000 / AU;

    void set_offset(Vector2 o) { m_offset = o; }
    void set_zoom(double d) { m_zoom = d; }
    Vector2 offset() const { return m_offset; }
    double scale() const { return m_zoom * SCALE; }
    void apply_zoom(double v) { m_zoom *= v; }

    Vector2 world_to_screen(Vector2 v) const { return (v - m_offset) * scale() + Vector2(window().getSize() / 2u); }
    Vector2 screen_to_world(Vector2 v) const { return (v - Vector2(window().getSize() / 2u)) / scale() + m_offset; }

    void reset()
    {
        m_offset = Vector2 {};
        m_zoom = 1;
    };

private:
    virtual void handle_event(sf::Event& event) override;
    virtual void draw(sf::RenderWindow&) const override;
    virtual void update() override;

    Vector2 m_offset;
    World& m_world;
    double m_zoom = 1;
    Object* m_focused_object = nullptr;
    unsigned m_clicks = 0;
    Vector2 m_prev_pos;
    bool m_dragging = false;
};
