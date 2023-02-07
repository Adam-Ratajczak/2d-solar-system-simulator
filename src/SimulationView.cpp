#include "SimulationView.hpp"

#include "World.hpp"
#include "glwrapper/Helpers.hpp"
#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/Text.hpp>

#include <Essa/GUI/NotifyUser.hpp>
#include <Essa/GUI/TextAlign.hpp>
#include <Essa/GUI/WidgetTreeRoot.hpp>
#include <Essa/LLGL/Core/Transform.hpp>
#include <Essa/LLGL/OpenGL/PrimitiveType.hpp>
#include <Essa/LLGL/OpenGL/Projection.hpp>
#include <Essa/LLGL/OpenGL/Shader.hpp>
#include <Essa/LLGL/OpenGL/Vertex.hpp>
#include <EssaUtil/DelayedInit.hpp>
#include <EssaUtil/Math/Ray.hpp>
#include <EssaUtil/UnitDisplay.hpp>
#include <EssaUtil/Vector.hpp>

#include <Essa/LLGL/Window/Mouse.hpp>
#include <cmath>
#include <optional>
#include <sstream>

GUI::Widget::EventHandlerResult SimulationView::on_mouse_button_press(GUI::Event::MouseButtonPress const& event) {
    m_prev_mouse_pos = event.local_position().cast<float>().to_deprecated_vector();
    // std::cout << "SV MouseButtonPressed " << Vector3 { m_prev_mouse_pos } << "b=" << (int)event.event().mouseButton.button << std::endl;
    m_prev_drag_pos = m_prev_mouse_pos;
    if (event.button() == llgl::MouseButton::Left) {
        m_drag_mode = DragMode::Pan;

        m_world.for_each_object([&](Object& obj) {
            auto obj_pos_screen = world_to_screen(obj.render_position());
            auto distance = Util::get_distance(Util::Vector2f { obj_pos_screen.x(), obj_pos_screen.y() }, m_prev_mouse_pos);
            if (distance < 30) {
                set_focused_object(&obj, GUI::NotifyUser::Yes);
                return;
            }
        });

        if (m_measure == Measure::Coords)
            m_measure = Measure::None;
        if (m_measure == Measure::Focus && m_focused_object != nullptr) {
            m_measure = Measure::None;
            m_measured = true;
            if (m_on_focus_measure) {
                m_on_focus_measure(m_focused_object);
            }
            m_focused_object = nullptr;
        }

        return EventHandlerResult::Accepted;
    }
    else if (llgl::is_mouse_button_pressed(llgl::MouseButton::Right)) {
        m_measure = Measure::None;
        m_drag_mode = DragMode::Rotate;
    }
    return EventHandlerResult::NotAccepted;
}

GUI::Widget::EventHandlerResult SimulationView::on_mouse_button_release(GUI::Event::MouseButtonRelease const& event) {
    m_is_dragging = false;
    m_drag_mode = DragMode::None;
    return EventHandlerResult::NotAccepted;
}

GUI::Widget::EventHandlerResult SimulationView::on_mouse_scroll(GUI::Event::MouseScroll const& event) {
    // TODO: Check mouse wheel
    if (event.delta() > 0)
        apply_zoom(1 / 1.1);
    else
        apply_zoom(1.1);
    return EventHandlerResult::NotAccepted;
}

GUI::Widget::EventHandlerResult SimulationView::on_mouse_move(GUI::Event::MouseMove const& event) {
    auto mouse_pos = event.local_position().cast<float>();
    m_prev_mouse_pos = mouse_pos.to_deprecated_vector();
    // std::cout << "SV MouseMoved " << Vector3 { m_prev_mouse_pos } << std::endl;

    // DRAG
    auto drag_delta = m_prev_drag_pos - mouse_pos.to_deprecated_vector();

    if (m_drag_mode != DragMode::None && !m_is_dragging && (std::abs(drag_delta.x()) > 20 || std::abs(drag_delta.y()) > 20)) {
        m_is_dragging = true;

        if (m_drag_mode == DragMode::Pan && m_focused_object) {
            m_focused_object = nullptr;
            m_offset.z() = 0;
        }
    }

    if (m_is_dragging) {
        switch (m_drag_mode) {
        case DragMode::Pan: {
            Util::Vector3d qad_delta { -drag_delta.x(), drag_delta.y(), 0 };
            set_offset(offset()
                + (llgl::Transform {}
                        .rotate_z(real_yaw())
                        .transform_point(Util::Cs::Point3f::from_deprecated_vector(qad_delta))
                        .to_vector()
                    * scale() / 320)
                      .cast<double>()
                      .to_deprecated_vector());
            break;
        }
        case DragMode::Rotate: {
            auto sizes = raw_size();
            m_yaw += drag_delta.x() / sizes.x() * M_PI;
            m_pitch -= drag_delta.y() / sizes.y() * M_PI;
            break;
        }
        default:
            break;
        }
        m_prev_drag_pos = mouse_pos.to_deprecated_vector();
    }

    // Coord measure
    if (m_measure == Measure::Coords) {
        auto object_pos_in_world_space = screen_to_world_on_grid(mouse_pos.to_deprecated_vector());
        if (object_pos_in_world_space) {
            m_measured = true;
            if (m_on_coord_measure) {
                m_on_coord_measure(*object_pos_in_world_space * Util::Constants::AU);
            }
        }
    }
    return EventHandlerResult::NotAccepted;
}

GUI::Widget::EventHandlerResult SimulationView::on_key_press(GUI::Event::KeyPress const& event) {
    if (event.modifiers().shift) {
        if (m_speed != 0)
            return GUI::Widget::EventHandlerResult::NotAccepted;
        if (event.code() == llgl::KeyCode::Right)
            m_world.update(1);
        else if (event.code() == llgl::KeyCode::Left)
            m_world.update(-1);
    }
    else {
        if (event.code() == llgl::KeyCode::Right) {
            if (m_speed > 0)
                m_speed *= 2;
            else if (m_speed == 0) {
                m_speed = 1;
                pop_pause();
            }
            else
                m_speed /= 2;

            if (m_speed == 0)
                push_pause();
        }
        else if (event.code() == llgl::KeyCode::Left) {
            if (m_speed < 0)
                m_speed *= 2;
            else if (m_speed == 0) {
                m_speed = -1;
                pop_pause();
            }
            else
                m_speed /= 2;

            if (m_speed == 0)
                push_pause();
        }
    }
    return EventHandlerResult::NotAccepted;
}

std::optional<Util::Vector3d> SimulationView::screen_to_world_on_grid(Util::Vector2f screen) const {
    auto clip_space = screen_to_clip_space(screen);
    Util::Math::Ray ray { { clip_space.x(), clip_space.y(), 0 }, { clip_space.x(), clip_space.y(), 1 } };

    // Transform a grid plane (z = 0) to the clip space.
    Util::Math::Plane plane = Util::Math::Plane(0, 0, 1, 0).transformed(matrix());
    /// std::cout << "[Coord] Mouse(Clip): " << clip_space << std::endl;

    // Calculate intersection of mouse ray and the grid. This will be our object position in clip space.
    auto object_pos_in_clip_space = ray.intersection_with_plane(plane);
    if (object_pos_in_clip_space) {
        // Go back to world coordinates to get actual object position.
        auto object_pos_in_world_space = llgl::Transform { matrix().inverted().convert<float>() }
                                             .transform_point(Util::Cs::Point3f::from_deprecated_vector(object_pos_in_clip_space.value()));

        // std::cout << "[Coord] Object(Clip)->Object(World): " << *object_pos_in_clip_space << " -> " << object_pos_in_world_space << std::endl;

        return object_pos_in_world_space.cast<double>().to_deprecated_vector();
    }
    return {};
}

void SimulationView::set_focused_object(Object* obj, GUI::NotifyUser notify_user) {
    m_focused_object = obj;
    if (notify_user == GUI::NotifyUser::Yes && m_focused_object && on_change_focus)
        on_change_focus(m_focused_object);
}

void SimulationView::draw_grid(Gfx::Painter& painter) const {
    constexpr float zoom_step_exponent = 2;
    auto spacing = std::pow(zoom_step_exponent, std::round(std::log2(m_zoom / 2) / std::log2(zoom_step_exponent)));
    constexpr int major_gridline_interval = 4;
    auto major_gridline_spacing = spacing * major_gridline_interval;
    {
        GUI::WorldDrawScope scope(painter);
        float bounds = 50 * spacing;
        // Vector3 start_coords = screen_to_world({ 0, 0 });
        // start_coords.x -= std::remainder(start_coords.x, spacing * major_gridline_interval) + spacing;
        // start_coords.y -= std::remainder(start_coords.y, spacing * major_gridline_interval) + spacing;
        // Vector3 end_coords = screen_to_world({ size().x, size().y });
        Util::Vector3d start_coords = { -bounds, -bounds, 0 };
        start_coords.x() -= std::round(m_offset.x() / major_gridline_spacing) * major_gridline_spacing;
        start_coords.y() -= std::round(m_offset.y() / major_gridline_spacing) * major_gridline_spacing;
        Util::Vector3d end_coords = { bounds, bounds, 0 };
        end_coords.x() -= std::round(m_offset.x() / major_gridline_spacing) * major_gridline_spacing;
        end_coords.y() -= std::round(m_offset.y() / major_gridline_spacing) * major_gridline_spacing;
        Util::Vector3d center_coords = (start_coords + end_coords) / 2.0;

        Util::Color const major_grid_line_color { 87, 87, 108 };
        Util::Color const grid_line_color { 25, 25, 37 };

        static Essa::Shaders::Basic shader;
        Essa::Shaders::Basic::Uniforms uniforms;
        uniforms.set_transform({},
            camera().view_matrix(),
            projection().matrix());

        std::vector<Essa::Shaders::Basic::Vertex> vertices;

        int index = 0;

        auto blend_color = [](Util::Color start, Util::Color end, float fac) {
            return Util::Color {
                static_cast<uint8_t>(start.r * (1 - fac) + end.r * fac),
                static_cast<uint8_t>(start.g * (1 - fac) + end.g * fac),
                static_cast<uint8_t>(start.b * (1 - fac) + end.b * fac),
                static_cast<uint8_t>(start.a * (1 - fac) + end.a * fac),
            };
        };

        // FIXME: Calculate bounds depending on window size instead of hardcoding them
        // TODO: Add real fog shader instead of THIS thing

        for (double x = start_coords.x(); x < end_coords.x(); x += spacing) {
            auto color = index % major_gridline_interval == 2 ? major_grid_line_color : grid_line_color;
            vertices.push_back({ { static_cast<float>(x), static_cast<float>(start_coords.y()), 0 }, Util::Colors::Transparent, {} });
            double factor = std::abs(0.5 - (x - start_coords.x()) / (end_coords.x() - start_coords.x())) * 2;
            auto center_color = blend_color(color, Util::Colors::Transparent, factor);
            vertices.push_back({ { static_cast<float>(x), static_cast<float>(center_coords.y()), 0 }, center_color, {} });
            // FIXME: Make this duplicate vertex not needed
            vertices.push_back({ { static_cast<float>(x), static_cast<float>(center_coords.y()), 0 }, center_color, {} });
            vertices.push_back({ { static_cast<float>(x), static_cast<float>(end_coords.y()), 0 }, Util::Colors::Transparent, {} });
            index++;
        }
        index = 0;
        for (double y = start_coords.y(); y < end_coords.y(); y += spacing) {
            auto color = index % major_gridline_interval == 2 ? major_grid_line_color : grid_line_color;
            vertices.push_back({ { static_cast<float>(start_coords.x()), static_cast<float>(y), 0 }, Util::Colors::Transparent, {} });
            double factor = std::abs(0.5 - (y - start_coords.y()) / (end_coords.y() - start_coords.y())) * 2;
            auto center_color = blend_color(color, Util::Colors::Transparent, factor);
            vertices.push_back({ { static_cast<float>(center_coords.x()), static_cast<float>(y), 0 }, center_color, {} });
            // FIXME: Make this duplicate vertex not needed
            vertices.push_back({ { static_cast<float>(center_coords.x()), static_cast<float>(y), 0 }, center_color, {} });
            vertices.push_back({ { static_cast<float>(end_coords.x()), static_cast<float>(y), 0 }, Util::Colors::Transparent, {} });
            index++;
        }

        GL::draw_with_temporary_vao<Essa::Shaders::Basic::Vertex>(painter.renderer(), shader, uniforms, llgl::PrimitiveType::Lines, vertices);
    }

    // guide
    Util::Cs::Point2f guide_start { raw_size().x() - 200.f, raw_size().y() - 30.f };
    // HACK: this *100 should be calculated from perspective somehow
    Util::Cs::Point2f guide_end = guide_start - Util::Cs::Vector2f(spacing * 300 / scale(), 0);
    std::array<Gfx::Vertex, 6> guide;
    Util::Color const guide_color { 127, 127, 127 };
    guide[0] = { (guide_start).to_deprecated_vector(), guide_color, {} };
    guide[1] = { (guide_end).to_deprecated_vector(), guide_color, {} };
    guide[2] = { (guide_start - Util::Cs::Vector2f(0, 5)).to_deprecated_vector(), guide_color, {} };
    guide[3] = { (guide_start + Util::Cs::Vector2f(0, 5)).to_deprecated_vector(), guide_color, {} };
    guide[4] = { (guide_end - Util::Cs::Vector2f(0, 5)).to_deprecated_vector(), guide_color, {} };
    guide[5] = { (guide_end + Util::Cs::Vector2f(0, 5)).to_deprecated_vector(), guide_color, {} };
    painter.draw_vertices(llgl::PrimitiveType::Lines, guide);

    // FIXME: UB on size_t conversion
    Gfx::Text guide_text { Util::unit_display(spacing / 2 / zoom_step_exponent * Util::Constants::AU, Util::Quantity::Length).to_string(), GUI::Application::the().font() };
    guide_text.set_font_size(theme().label_font_size);
    guide_text.set_fill_color(Util::Colors::White);
    guide_text.align(GUI::Align::Center, { guide_start - Util::Cs::Vector2f(0, 10), (guide_end - guide_start).to_size() });
    guide_text.draw(painter);
}

llgl::Camera SimulationView::camera() const {
    return llgl::Camera { projection() }
        .translate(Util::Vector3f { -m_offset })
        .rotate_z(Util::Angle::radians(static_cast<float>(m_yaw)))
        .rotate_x(Util::Angle::radians(static_cast<float>(m_pitch)))
        .translate({ 0, 0, scale() });
}

llgl::Projection SimulationView::projection() const {
    return llgl::Projection::perspective({ m_fov.rad(), raw_size().x() / raw_size().y(), 0.1 * scale(), 1000 * scale() }, Util::Recti { rect() });
}

Util::Matrix4x4d SimulationView::matrix() const {
    return (projection().matrix() * camera().view_matrix()).convert<double>();
}

Util::Vector3f SimulationView::world_to_screen(Util::Vector3d local_space) const {
    // https://learnopengl.com/Getting-started/Coordinate-Systems
    auto clip_space = (matrix() * Util::Cs::Point4d { Util::Cs::Point3d::from_deprecated_vector(local_space), 1 }).to_vector();
    clip_space /= clip_space.w();
    return Util::Vector3f { clip_space_to_screen(Util::Cs::Vector3d { clip_space }.to_deprecated_vector()), clip_space.z() };
}

llgl::Renderer& SimulationView::renderer() const {
    return host_window().renderer();
}

void SimulationView::draw(Gfx::Painter& window) const {
    if (m_show_grid)
        draw_grid(window);
    m_world.draw(window, *this);

    switch (m_measure) {
    case Measure::Focus: {
        auto sizes = raw_size();
        std::array<Gfx::Vertex, 4> lines;
        lines[0] = Gfx::Vertex { { 0, static_cast<float>(m_prev_mouse_pos.y()) }, Util::Colors::Green, {} };
        lines[1] = Gfx::Vertex { { sizes.x(), static_cast<float>(m_prev_mouse_pos.y()) }, Util::Colors::Green, {} };
        lines[2] = Gfx::Vertex { { static_cast<float>(m_prev_mouse_pos.x()), 0 }, Util::Colors::Green, {} };
        lines[3] = Gfx::Vertex { { static_cast<float>(m_prev_mouse_pos.x()), sizes.y() }, Util::Colors::Green, {} };
        window.draw_vertices(llgl::PrimitiveType::Lines, lines);
        break;
    }
    default:
        break;
    }

    if (m_display_debug_info) {
        std::ostringstream debug_oss;
        auto mp = llgl::mouse_position();
        debug_oss << "s=" << scale() << std::endl;
        debug_oss << "off=" << offset() << std::endl;
        debug_oss << "yaw=" << m_yaw << " $ " << m_yaw_from_object << std::endl;
        debug_oss << "pitch=" << m_pitch << " $ " << m_pitch_from_object << std::endl;
        debug_oss << "pause_count=" << m_pause_count << std::endl;

        Gfx::Text debug_text { Util::UString { debug_oss.str() }, GUI::Application::the().fixed_width_font() };
        debug_text.set_fill_color(Util::Colors::White);
        debug_text.set_font_size(theme().label_font_size);
        debug_text.set_position({ 600, 20 });
        debug_text.draw(window);
    }
}

void SimulationView::push_pause() {
    m_pause_count++;
}

void SimulationView::pop_pause() {
    assert(m_pause_count > 0);
    m_pause_count--;
}

void SimulationView::update() {
    // FIXME: This doesn't quite match here like speed (The same
    //        comment about Simulation object)
    if (!is_paused())
        m_world.update(speed() * m_iterations);

    // Handle focus
    if (m_focused_object) {
        set_offset(-m_focused_object->render_position());

        if (m_focused_object->most_attracting_object() && m_fixed_rotation_on_focus) {
            Util::Vector3d a = m_focused_object->pos() - m_focused_object->most_attracting_object()->pos();

            m_pitch_from_object = std::atan2(a.y(), a.z()) - M_PI / 2;
            m_yaw_from_object = std::atan2(a.y(), a.x()) + M_PI / 2;

            if (a.y() < 0)
                m_pitch_from_object -= M_PI;
        }
    }

    if (m_focused_object != nullptr && if_focused)
        if_focused();
}

Object* SimulationView::focused_object() const {
    if (m_focused_object != nullptr)
        return m_focused_object;
    return {};
}

Util::Vector2f SimulationView::clip_space_to_screen(Util::Vector3d clip_space) const {
    return { (clip_space.x() + 1) / 2 * raw_size().x(), raw_size().y() - (clip_space.y() + 1) / 2 * raw_size().y() };
}

Util::Vector3d SimulationView::screen_to_clip_space(Util::Vector2f viewport) const {
    return { (viewport.x() - raw_size().x() / 2.0) * 2 / raw_size().x(), -(viewport.y() - raw_size().y() / 2.0) * 2 / raw_size().y(), 1 };
}

#ifdef ENABLE_PYSSA

void SimulationView::setup_python_bindings(TypeSetup type_setup) {
    type_setup.add_method<&SimulationView::python_reset>("reset", "Reset view transform");
    type_setup.add_attribute<&SimulationView::python_get_offset, &SimulationView::python_set_offset>("offset");
    type_setup.add_attribute<&SimulationView::python_get_fov, &SimulationView::python_set_fov>("fov", "Field of view");
    type_setup.add_attribute<&SimulationView::python_get_yaw, &SimulationView::python_set_yaw>("yaw");
    type_setup.add_attribute<&SimulationView::python_get_pitch, &SimulationView::python_set_pitch>("pitch");
    type_setup.add_attribute<&SimulationView::python_get_world, nullptr>("world");
    type_setup.add_attribute<&SimulationView::python_get_zoom, &SimulationView::python_set_zoom>("zoom");
    type_setup.add_attribute<&SimulationView::python_get_focused_object, &SimulationView::python_set_focused_object>("focused_object");
}

PySSA::Object SimulationView::python_reset(PySSA::Object const&, PySSA::Object const&) {
    reset();
    return PySSA::Object::none();
}

PySSA::Object SimulationView::python_get_offset() const {
    return PySSA::Object::create(m_offset);
}

bool SimulationView::python_set_offset(PySSA::Object const& object) {
    auto v = object.as_vector();
    if (!v)
        return false;
    m_offset = *v;
    return true;
}

PySSA::Object SimulationView::python_get_fov() const {
    return PySSA::Object::create(m_fov.deg());
}

bool SimulationView::python_set_fov(PySSA::Object const& object) {
    auto v = object.as_double();
    if (!v)
        return false;
    m_fov = Util::Angle::degrees(*v);
    return true;
}

PySSA::Object SimulationView::python_get_yaw() const {
    return PySSA::Object::create(m_yaw);
}

bool SimulationView::python_set_yaw(PySSA::Object const& object) {
    auto v = object.as_double();
    if (!v)
        return false;
    m_yaw = *v;
    return true;
}

PySSA::Object SimulationView::python_get_pitch() const {
    return PySSA::Object::create(m_pitch);
}

bool SimulationView::python_set_pitch(PySSA::Object const& object) {
    auto v = object.as_double();
    if (!v)
        return false;
    m_pitch = *v;
    return true;
}

PySSA::Object SimulationView::python_get_world() const {
    return m_world.wrap();
}

PySSA::Object SimulationView::python_get_zoom() const {
    return PySSA::Object::create(m_zoom);
}

bool SimulationView::python_set_zoom(PySSA::Object const& object) {
    auto v = object.as_double();
    if (!v)
        return false;
    m_zoom = *v;
    return true;
}

PySSA::Object SimulationView::python_get_focused_object() const {
    return m_focused_object ? m_focused_object->wrap() : PySSA::Object::none();
}

bool SimulationView::python_set_focused_object(PySSA::Object const& object) {
    auto v = Object::get(object);
    if (!v)
        return false;
    set_focused_object(v);
    return true;
}

#endif
