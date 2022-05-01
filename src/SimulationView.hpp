#pragma once

#include "gui/Widget.hpp"
#include "math/Matrix.hpp"
#include "math/Transform.hpp"
#include "math/Vector3.hpp"
#include "pyssa/WrappedObject.hpp"
#include "util/Constants.hpp"
#include <SFML/Graphics.hpp>
#include <functional>
#include <optional>

class Object;
class World;

class SimulationView : public GUI::Widget
    , public PySSA::WrappedObject<SimulationView> {
public:
    explicit SimulationView(GUI::Container& c, World& world)
        : Widget(c)
        , m_world(world) { reset(); }

    void set_offset(Vector3 o) { m_offset = o; }
    void set_zoom(double d) { m_zoom = d; }
    Vector3 offset() const { return m_offset; }
    double scale() const { return m_zoom; }
    void apply_zoom(double v) { m_zoom *= v; }
    void reset_rotation() {
        m_pitch = -0.7;
        m_yaw = 0;
    }

    Vector3 screen_to_world(Vector3 v) const;
    Vector3 world_to_screen(Vector3 v) const;
    Matrix4x4d projection_matrix() const;
    Matrix4x4d modelview_matrix() const;
    Matrix4x4d matrix() const;

    void reset() {
        m_offset = Vector3 { 0, 0, 0 };
        m_zoom = 1;
        m_focused_object = nullptr;
        reset_rotation();
    };

    void start_coords_measure(std::function<void(Vector3)> handler) {
        m_measure = Measure::Coords;
        m_on_coord_measure = std::move(handler);
    }

    void start_focus_measure(std::function<void(Object*)> handler) {
        m_measure = Measure::Focus;
        m_on_focus_measure = std::move(handler);
    }

    std::function<void(Object*)> on_change_focus;
    std::function<void()> if_focused;

    void toggle_label_visibility(bool visibility) { m_show_labels = visibility; }
    bool show_labels() const { return m_show_labels; }
    void set_show_grid(bool b) { m_show_grid = b; }
    void set_show_trails(bool b) { m_show_trails = b; }
    bool show_trails() const { return m_show_trails; }
    void set_offset_trails(bool b) { m_offset_trail = b; }
    bool offset_trails() const { return m_offset_trail; }

    void set_fov(Angle fov) { m_fov = fov; }

    // TODO: This should be private
    bool m_measured = false;

    // FIXME: This should be in GUI.
    void set_fps(float fps) { m_fps = fps; }

    // FIXME: This should be in some Simulation object.
    int speed() const { return m_speed; }
    void set_speed(int speed) { m_speed = speed; }

    int iterations() const { return m_iterations; }
    void set_iterations(int i) { m_iterations = i; }

    World& world() { return m_world; }
    Object* focused_object() const;
    void set_focused(Object* obj) { m_focused_object = obj; }

    void change_speed(bool state) { m_allow_change_speed = state; }

    static void setup_python_bindings(TypeSetup);
    static constexpr char const* PythonClassName = "SimulationView";

private:
    virtual void handle_event(GUI::Event&) override;
    virtual void draw(sf::RenderWindow&) const override;
    virtual void update() override;

    virtual bool accepts_focus() const override { return true; }

    void draw_grid(sf::RenderWindow&) const;

    PySSA::Object python_reset(PySSA::Object const& args);

    PySSA::Object python_get_offset() const;
    PySSA::Object python_get_fov() const;
    PySSA::Object python_get_yaw() const;
    PySSA::Object python_get_pitch() const;
    PySSA::Object python_get_world() const;
    PySSA::Object python_get_zoom() const;
    PySSA::Object python_get_focused_object() const;

    Vector3 m_offset;
    Angle m_fov = 80.0_deg;
    double m_yaw = 0;
    double m_pitch = 0;

    World& m_world;
    double m_zoom = 1;
    Object* m_focused_object = nullptr;
    Object* m_prev_focused_object = nullptr;
    unsigned m_clicks = 0;
    sf::Vector2f m_prev_mouse_pos;
    sf::Vector2f m_prev_drag_pos;

    std::function<void(Vector3 pos)> m_on_coord_measure;
    std::function<void(Object* focused)> m_on_focus_measure;

    enum class DragMode {
        None,
        Pan,    // left click
        Rotate, // right click
    };
    DragMode m_drag_mode {};
    bool m_is_dragging { false };

    enum class Measure {
        None,
        Coords,
        Focus
    };

    Measure m_measure = Measure::None;

    bool m_show_labels = true;
    bool m_show_grid = true;
    bool m_show_trails = true;
    bool m_offset_trail = true;

    bool m_allow_change_speed = true;

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
    enum class ClearDepth {
        Yes,
        No
    };

    explicit WorldDrawScope(SimulationView const& view, ClearDepth = ClearDepth::No);
    ~WorldDrawScope();

    static void verify();

private:
    SimulationView const& m_simulation_view;
};
