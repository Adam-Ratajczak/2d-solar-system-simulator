#pragma once

#include "../Constants.hpp"
#include "../math/Matrix.hpp"
#include "../math/Vector3.hpp"
#include "../math/Transform.hpp"
#include "Widget.hpp"
#include <SFML/Graphics.hpp>
#include <functional>

class Object;
class World;

class SimulationView : public Widget {
public:
    explicit SimulationView(Container& c, World& world)
        : Widget(c)
        , m_world(world) { }

    void set_offset(Vector3 o) { m_offset = o; }
    void set_zoom(double d) { m_zoom = d; }
    Vector3 offset() const { return m_offset; }
    double scale() const { return m_zoom; }
    void apply_zoom(double v) { m_zoom *= v; }
    void reset_rotation(){ m_rotate_x = 0.7; m_rotate_y = 0; m_rotate_z = 0;}

    Vector3 screen_to_world(Vector3 v) const;
    Vector3 world_to_screen(Vector3 v) const;
    Matrix4x4d projection_matrix() const;
    Matrix4x4d modelview_matrix() const;

    void reset() {
        m_offset = Vector3 {0, 0, 0};
        m_zoom = 1;
    };

    std::function<void(Vector3 pos)> on_coord_measure;
    std::function<void(Object* focused)> on_focus_measure;

    void start_coords_measure() { m_coord_measure = true; }
    void start_focus_measure();
    void toggle_label_visibility(){ m_show_labels = !m_show_labels;}
    bool show_labels() const{return m_show_labels;}

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
    double m_rotate_x = 0.7, m_rotate_y = 0, m_rotate_z = 0;
    Object* m_focused_object = nullptr;
    unsigned m_clicks = 0;
    sf::Vector2f m_prev_mouse_pos;
    Vector3 m_prev_pos;
    bool m_dragging = false, m_rotating = false;
    bool m_coord_measure = false, m_focus_measure = false, m_show_labels = true;
    int m_iterations = 10;

    // FIXME: This doesn't quite match here (and also World). Maybe
    //        add some Simulation class.
    int m_speed = 1;

    // FIXME: This should be in GUI.
    float m_fps = 60;
};

// This class ensures that everything in the scope will be drawn using
// world transform (that is, in 3D, with depth enabled), Also, it takes
// care of restoring all OpenGL states so that you can use SFML safely
// after the scope.
class WorldDrawScope {
public:
    // Can be used for doing multiple layers.
    enum class ClearDepth
    {
        Yes,
        No
    };

    explicit WorldDrawScope(SimulationView const& view, ClearDepth = ClearDepth::No);
    ~WorldDrawScope();
 
    static void verify();

private:
    SimulationView const& m_simulation_view;
};
