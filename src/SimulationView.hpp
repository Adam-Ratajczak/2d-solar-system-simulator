#pragma once

#include <EssaGUI/gui/NotifyUser.hpp>
#include <GL/glew.h>

#include "math/Transform.hpp"
#include "pyssa/WrappedObject.hpp"
#include <EssaGUI/gui/Widget.hpp>
#include <EssaGUI/util/Constants.hpp>
#include <EssaGUI/util/Matrix.hpp>
#include <EssaGUI/util/Vector3.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Shader.hpp>
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
        m_pitch_from_object = 0;
        m_yaw = 0;
        m_yaw_from_object = 0;
    }

    double real_yaw() const { return m_yaw + m_yaw_from_object; }
    double real_pitch() const { return m_pitch + m_pitch_from_object; }

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
    void set_fixed_rotation_on_focus(bool b) { m_fixed_rotation_on_focus = b; }
    bool set_fixed_rotation_on_focus() const { return m_fixed_rotation_on_focus; }

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
    void set_focused_object(Object* obj, GUI::NotifyUser notify_user = GUI::NotifyUser::No);

    void change_speed(bool state) { m_allow_change_speed = state; }
    void pause_simulation(bool state);

    static void setup_python_bindings(TypeSetup);
    static constexpr char const* PythonClassName = "SimulationView";

private:
    virtual void handle_event(GUI::Event&) override;
    virtual void draw(GUI::SFMLWindow&) const override;
    virtual void update() override;

    virtual bool accepts_focus() const override { return true; }

    void draw_grid(GUI::SFMLWindow&) const;

    PySSA::Object python_reset(PySSA::Object const& args, PySSA::Object const& kwargs);

    PySSA::Object python_get_offset() const;
    bool python_set_offset(PySSA::Object const&);
    PySSA::Object python_get_fov() const;
    bool python_set_fov(PySSA::Object const&);
    PySSA::Object python_get_yaw() const;
    bool python_set_yaw(PySSA::Object const&);
    PySSA::Object python_get_pitch() const;
    bool python_set_pitch(PySSA::Object const&);
    PySSA::Object python_get_world() const;
    // No setter for world
    PySSA::Object python_get_zoom() const;
    bool python_set_zoom(PySSA::Object const&);
    PySSA::Object python_get_focused_object() const;
    bool python_set_focused_object(PySSA::Object const&);

    Vector3 m_offset;
    Angle m_fov = 80.0_deg;

    double m_yaw = 0;
    double m_pitch = 0;
    double m_yaw_from_object = 0;
    double m_pitch_from_object = 0;

    World& m_world;
    double m_zoom = 1;
    Object* m_focused_object = nullptr;
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
    bool m_fixed_rotation_on_focus = true;

    bool m_allow_change_speed = true;

    int m_iterations = 1;

    // FIXME: This doesn't quite match here (and also World). Maybe
    //        add some Simulation class.
    int m_speed = 1, m_saved_speed = 1;

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

    // Custom shader uniforms:
    // - modelviewMatrix : mat4
    // - projectionMatrix : mat4
    // - the same as for default shader (see WorldShader.hpp)
    explicit WorldDrawScope(SimulationView const& view, ClearDepth = ClearDepth::No, sf::Shader* custom_shader = nullptr);
    ~WorldDrawScope();

    // Re-apply all uniforms to the shader This can be used when you
    // draw things with another shaders in a single WorldDrawScope.
    // (Actually this is a HACK because Sphere doesn't know about SV)
    void apply_uniforms(sf::Shader&) const;

    static void verify();
    static WorldDrawScope const* current();

private:
    SimulationView const& m_simulation_view;
    WorldDrawScope const* m_parent = nullptr;
};
