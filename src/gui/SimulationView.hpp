#pragma once

#include "../Constants.hpp"
#include "../Vector2.hpp"
#include "Widget.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <functional>

class Object;
class World;

class SimulationView : public Widget {
public:
    explicit SimulationView(Container& c, World& world)
        : Widget(c)
        , m_world(world) { }

    static constexpr double SCALE = 1000 / AU;

    void set_offset(Vector2 o) { m_offset = o; }
    void set_zoom(double d) { m_zoom = d; }
    Vector2 offset() const { return m_offset; }
    double scale() const { return m_zoom * SCALE; }
    void apply_zoom(double v) { m_zoom *= v; }

    Vector2 world_to_screen(Vector2 v) const { return (v - m_offset) * scale() + Vector2(window().getSize() / 2u); }
    Vector2 screen_to_world(Vector2 v) const { return (v - Vector2(window().getSize() / 2u)) / scale() + m_offset; }

    void reset() {
        m_offset = Vector2 {};
        m_zoom = 1;
    };

    std::function<void(sf::Vector2f pos)> on_coord_measure;
    std::function<void(Object* focused)> on_focus_measure;

    void start_coords_measure() { m_coord_measure = true; }
    void start_focus_measure();

    // TODO: This should be private
    bool m_measured = false, m_changed = false;

    // FIXME: This should be in GUI.
    void set_fps(float fps) { m_fps = fps; }

private:
    virtual void handle_event(Event&) override;
    virtual void draw(sf::RenderWindow&) const override;
    virtual void update() override;

    void draw_grid(sf::RenderWindow&) const;

    Vector2 m_offset;
    World& m_world;
    double m_zoom = 1;
    Object* m_focused_object = nullptr;
    unsigned m_clicks = 0;
    sf::Vector2f m_prev_mouse_pos;
    Vector2 m_prev_pos;
    bool m_dragging = false;
    bool m_coord_measure = false, m_focus_measure = false;

    // FIXME: This doesn't quite match here (and also World). Maybe
    //        add some Simulation class.
    int m_speed = 1;
    float m_fps = 60;
};
