#pragma once

#include "../Constants.hpp"
#include "../Vector3.hpp"
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

    void set_offset(Vector3 o) { m_offset = o; }
    void set_zoom(double d) { m_zoom = d; }
    Vector3 offset() const { return m_offset; }
    double scale() const { return m_zoom * SCALE; }
    void apply_zoom(double v) { m_zoom *= v; }

    Vector3 world_to_screen(Vector3 v) const { return (v - m_offset) * scale() + Vector3(window().getSize() / 2u); }
    Vector3 screen_to_world(Vector3 v) const { return (v - Vector3(window().getSize() / 2u)) / scale() + m_offset; }

    void reset() {
        m_offset = Vector3 {};
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

    // FIXME: This should be in some Simulation object.
    int speed() const { return m_speed; }
    void set_speed(int speed) { m_speed = speed; }

    int iterations() const { return m_iterations; }
    void set_iterations(int i) { m_iterations = i; }

private:
    virtual void handle_event(Event&) override;
    virtual void draw(sf::RenderWindow&) const override;
    virtual void update() override;

    void draw_grid(sf::RenderWindow&) const;

    Vector3 m_offset;
    World& m_world;
    double m_zoom = 1;
    Object* m_focused_object = nullptr;
    unsigned m_clicks = 0;
    sf::Vector2f m_prev_mouse_pos;
    Vector3 m_prev_pos;
    bool m_dragging = false;
    bool m_coord_measure = false, m_focus_measure = false;
    int m_iterations = 10;

    // FIXME: This doesn't quite match here (and also World). Maybe
    //        add some Simulation class.
    int m_speed = 1;

    // FIXME: This should be in GUI.
    float m_fps = 60;
};
