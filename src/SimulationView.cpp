#include "SimulationView.hpp"

#include "World.hpp"
#include "WorldShader.hpp"
#include "math/Ray.hpp"
#include "math/Transform.hpp"

#include <EssaGUI/gfx/SFMLWindow.hpp>
#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/TextAlign.hpp>
#include <EssaGUI/gui/WidgetTreeRoot.hpp>
#include <EssaGUI/util/DelayedInit.hpp>
#include <EssaGUI/util/UnitDisplay.hpp>
#include <EssaGUI/util/Vector3.hpp>

#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <optional>
#include <sstream>

void SimulationView::handle_event(GUI::Event& event) {
    if (event.type() == sf::Event::MouseButtonPressed) {
        m_prev_mouse_pos = { static_cast<float>(event.event().mouseButton.x), static_cast<float>(event.event().mouseButton.y) };
        // std::cout << "SV MouseButtonPressed " << Vector3 { m_prev_mouse_pos } << "b=" << (int)event.event().mouseButton.button << std::endl;
        m_prev_drag_pos = m_prev_mouse_pos;
        if (event.event().mouseButton.button == sf::Mouse::Left) {
            m_drag_mode = DragMode::Pan;

            m_world.for_each_object([&](Object& obj) {
                auto obj_pos_screen = world_to_screen(obj.render_position());
                obj_pos_screen.z = 0;
                auto distance = obj_pos_screen.distance_to(Vector3(m_prev_mouse_pos));
                if (distance < 30) {
                    m_focused_object = &obj;
                    if (on_change_focus && m_prev_focused_object == m_focused_object)
                        on_change_focus(m_focused_object);
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
                m_yaw += m_yaw_from_object;
                m_pitch = m_pitch_from_object;
            }

            event.set_handled();
        }
        else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            m_measure = Measure::None;
            m_drag_mode = DragMode::Rotate;
        }
    }
    else if (event.type() == sf::Event::MouseWheelScrolled) {
        if (event.event().mouseWheelScroll.wheel != sf::Mouse::VerticalWheel)
            return;
        if (event.event().mouseWheelScroll.delta > 0)
            apply_zoom(1 / 1.1);
        else
            apply_zoom(1.1);
    }
    else if (event.type() == sf::Event::MouseMoved) {
        sf::Vector2f mouse_pos { static_cast<float>(event.event().mouseMove.x), static_cast<float>(event.event().mouseMove.y) };
        m_prev_mouse_pos = mouse_pos;
        // std::cout << "SV MouseMoved " << Vector3 { m_prev_mouse_pos } << std::endl;

        // DRAG
        auto drag_delta = m_prev_drag_pos - mouse_pos;

        if (m_drag_mode != DragMode::None && !m_is_dragging && (std::abs(drag_delta.x) > 20 || std::abs(drag_delta.y) > 20)) {
            m_is_dragging = true;

            if (m_drag_mode == DragMode::Pan && m_focused_object) {
                m_focused_object = nullptr;
                m_offset.z = 0;
            }
        }

        if (m_is_dragging) {
            switch (m_drag_mode) {
            case DragMode::Pan: {
                Vector3 qad_delta { -drag_delta.x, drag_delta.y, 0 };
                set_offset(offset() + (Transform::rotation_around_z(-real_yaw()) * qad_delta * scale() / 320));
                break;
            }
            case DragMode::Rotate: {
                auto sizes = window().getSize();
                m_yaw += drag_delta.x / sizes.x * M_PI;
                m_pitch -= drag_delta.y / sizes.y * M_PI;
                break;
            }
            default:
                break;
            }
            m_prev_drag_pos = mouse_pos;
        }

        // COORD MEASURE
        if (m_measure == Measure::Coords) {
            // FIXME: This doesn't work perfectly yet.
            Vector3 mouse_pos_in_clip_space { (mouse_pos.x - size().x / 2.0) * 2 / size().x, -(mouse_pos.y - size().y / 2.0) * 2 / size().y, 1, 1 };
            Math::Ray ray { {}, mouse_pos_in_clip_space };

            // Transform a grid plane (z = 0) to the clip space.
            Math::Plane plane = Math::Plane(0, 0, 1, 0).transformed(matrix());

            // Calculate intersection of mouse ray and the grid. This will be our object position in clip space.
            auto object_pos_in_clip_space = ray.intersection_with_plane(plane);
            if (object_pos_in_clip_space) {
                // Go back to world coordinates to get actual object position.
                auto object_pos_in_world_space = matrix().inverted() * object_pos_in_clip_space.value();

                // Inform the client about measured position.
                m_measured = true;
                if (m_on_coord_measure) {
                    m_on_coord_measure(object_pos_in_world_space * AU);
                }
            }
        }
    }
    else if (event.type() == sf::Event::MouseButtonReleased) {
        // std::cout << "SV MouseButtonReleased " << Vector3 { m_prev_mouse_pos } << "b=" << (int)event.event().mouseButton.button << std::endl;
        m_is_dragging = false;
        m_drag_mode = DragMode::None;
    }
    else if (event.type() == sf::Event::KeyPressed && m_allow_change_speed) {
        if (event.event().key.shift) {
            if (m_speed != 0)
                return;
            if (event.event().key.code == sf::Keyboard::Right)
                m_world.update(1);
            else if (event.event().key.code == sf::Keyboard::Left)
                m_world.update(-1);
        }
        else {
            if (event.event().key.code == sf::Keyboard::Right) {
                if (m_speed > 0)
                    m_speed *= 2;
                else if (m_speed == 0)
                    m_speed = 1;
                else
                    m_speed /= 2;
            }
            else if (event.event().key.code == sf::Keyboard::Left) {
                if (m_speed < 0)
                    m_speed *= 2;
                else if (m_speed == 0)
                    m_speed = -1;
                else
                    m_speed /= 2;
            }
        }
    }
}

void SimulationView::draw_grid(GUI::SFMLWindow& window) const {
    constexpr float zoom_step_exponent = 2;
    auto spacing = std::pow(zoom_step_exponent, std::round(std::log2(m_zoom / 2) / std::log2(zoom_step_exponent)));
    constexpr int major_gridline_interval = 4;
    auto major_gridline_spacing = spacing * major_gridline_interval;
    {
        WorldDrawScope scope(*this);
        float bounds = 50 * spacing;
        // Vector3 start_coords = screen_to_world({ 0, 0 });
        // start_coords.x -= std::remainder(start_coords.x, spacing * major_gridline_interval) + spacing;
        // start_coords.y -= std::remainder(start_coords.y, spacing * major_gridline_interval) + spacing;
        // Vector3 end_coords = screen_to_world({ size().x, size().y });
        Vector3 start_coords = { -bounds, -bounds, 0 };
        start_coords.x -= std::round(m_offset.x / major_gridline_spacing) * major_gridline_spacing;
        start_coords.y -= std::round(m_offset.y / major_gridline_spacing) * major_gridline_spacing;
        Vector3 end_coords = { bounds, bounds, 0 };
        end_coords.x -= std::round(m_offset.x / major_gridline_spacing) * major_gridline_spacing;
        end_coords.y -= std::round(m_offset.y / major_gridline_spacing) * major_gridline_spacing;
        Vector3 center_coords = (start_coords + end_coords) / 2.0;

        sf::Color const major_grid_line_color { 87, 87, 108 };
        sf::Color const grid_line_color { 25, 25, 37 };

        // TODO: Create proper API for it.
        std::vector<Vertex> vertices;

        int index = 0;

        auto blend_color = [](sf::Color start, sf::Color end, float fac) {
            return sf::Color {
                static_cast<uint8_t>(start.r * (1 - fac) + end.r * fac),
                static_cast<uint8_t>(start.g * (1 - fac) + end.g * fac),
                static_cast<uint8_t>(start.b * (1 - fac) + end.b * fac),
                static_cast<uint8_t>(start.a * (1 - fac) + end.a * fac),
            };
        };

        // FIXME: Calculate bounds depending on window size instead of hardcoding them
        // TODO: Add real fog shader instead of THIS thing
        for (double x = start_coords.x; x < end_coords.x; x += spacing) {
            auto color = index % major_gridline_interval == 2 ? major_grid_line_color : grid_line_color;
            vertices.push_back(Vertex { .position = { x, start_coords.y, 0 }, .color = sf::Color::Transparent });
            double factor = std::abs(0.5 - (x - start_coords.x) / (end_coords.x - start_coords.x)) * 2;
            auto center_color = blend_color(color, sf::Color::Transparent, factor);
            vertices.push_back(Vertex { .position = { x, center_coords.y, 0 }, .color = center_color });
            // FIXME: Make this duplicate vertex not needed
            vertices.push_back(Vertex { .position = { x, center_coords.y, 0 }, .color = center_color });
            vertices.push_back(Vertex { .position = { x, end_coords.y, 0 }, .color = sf::Color::Transparent });
            index++;
        }
        index = 0;
        for (double y = start_coords.y; y < end_coords.y; y += spacing) {
            auto color = index % major_gridline_interval == 2 ? major_grid_line_color : grid_line_color;
            vertices.push_back(Vertex { .position = { start_coords.x, y, 0 }, .color = sf::Color::Transparent });
            double factor = std::abs(0.5 - (y - start_coords.y) / (end_coords.y - start_coords.y)) * 2;
            auto center_color = blend_color(color, sf::Color::Transparent, factor);
            vertices.push_back(Vertex { .position = { center_coords.x, y, 0 }, .color = center_color });
            // FIXME: Make this duplicate vertex not needed
            vertices.push_back(Vertex { .position = { center_coords.x, y, 0 }, .color = center_color });
            vertices.push_back(Vertex { .position = { end_coords.x, y, 0 }, .color = sf::Color::Transparent });
            index++;
        }

        window.draw_vertices(GL_LINES, vertices);
    }

    // guide
    sf::Vector2f guide_start { size().x - 200.f, size().y - 30.f };
    // HACK: this *100 should be calculated from perspective somehow
    sf::Vector2f guide_end = guide_start - sf::Vector2f(spacing * 300 / scale(), 0);
    std::array<Vertex, 6> guide;
    sf::Color const guide_color { 127, 127, 127 };
    guide[0] = Vertex { .position = Vector3(guide_start), .color = guide_color };
    guide[1] = Vertex { .position = Vector3(guide_end), .color = guide_color };
    guide[2] = Vertex { .position = Vector3(guide_start - sf::Vector2f(0, 5)), .color = guide_color };
    guide[3] = Vertex { .position = Vector3(guide_start + sf::Vector2f(0, 5)), .color = guide_color };
    guide[4] = Vertex { .position = Vector3(guide_end - sf::Vector2f(0, 5)), .color = guide_color };
    guide[5] = Vertex { .position = Vector3(guide_end + sf::Vector2f(0, 5)), .color = guide_color };
    window.draw_vertices(GL_LINES, guide);

    // FIXME: UB on size_t conversion
    window.draw_text_aligned_in_rect(Util::unit_display(spacing / 2 / zoom_step_exponent * AU, Util::Quantity::Length).to_string(),
        { guide_start, {} }, GUI::Application::the().font);
}

Matrix4x4d SimulationView::projection_matrix() const {
    double fov = m_fov.rad();
    double fd = 1 / std::tan(fov / 2);
    double aspect = size().x / (double)size().y;
    double zFar = 1000 * scale();
    double zNear = 0.1 * scale();

    return { { { fd / aspect, 0, 0, 0 },
        { 0, fd, 0, 0 },
        { 0, 0, (zFar + zNear) / (zNear - zFar), -1 },
        { 0, 0, (2 * zFar * zNear) / (zNear - zFar), 0 } } };
}

Matrix4x4d SimulationView::modelview_matrix() const {
    Matrix4x4d matrix = Matrix4x4d::identity();
    matrix = matrix * Transform::translation(m_offset);
    matrix = matrix * Transform::rotation_around_z(real_yaw());
    matrix = matrix * Transform::rotation_around_x(real_pitch());
    matrix = matrix * Transform::translation({ 0, 0, -scale() });
    return matrix;
}

Matrix4x4d SimulationView::matrix() const {
    return modelview_matrix() * projection_matrix();
}

Vector3 SimulationView::screen_to_world(Vector3 v) const {
    Vector3 clip_space { -(v.x - size().x / 2.0) * 2 / size().x, (v.y - size().y / 2.0) * 2 / size().y };
    return matrix().inverted() * clip_space;
}

Vector3 SimulationView::world_to_screen(Vector3 local_space) const {
    // https://learnopengl.com/Getting-started/Coordinate-Systems
    auto clip_space = matrix() * local_space;
    return { (clip_space.x + 1) / 2 * size().x, (-clip_space.y + 1) / 2 * size().y, clip_space.z };
}

void SimulationView::draw(GUI::SFMLWindow& window) const {
    if (m_show_grid)
        draw_grid(window);
    m_world.draw(*this);

    switch (m_measure) {
    case Measure::Coords: {
        auto sizes = size();
        std::array<Vertex, 4> lines;
        lines[0] = Vertex { .position = Vector3 { 0, static_cast<float>(m_prev_mouse_pos.y) }, .color = sf::Color::Red };
        lines[1] = Vertex { .position = Vector3 { sizes.x, static_cast<float>(m_prev_mouse_pos.y) }, .color = sf::Color::Red };
        lines[2] = Vertex { .position = Vector3 { static_cast<float>(m_prev_mouse_pos.x), 0 }, .color = sf::Color::Red };
        lines[3] = Vertex { .position = Vector3 { static_cast<float>(m_prev_mouse_pos.x), sizes.y }, .color = sf::Color::Red };
        window.draw_vertices(GL_LINES, lines);
        break;
    }
    case Measure::Focus: {
        auto sizes = size();
        std::array<Vertex, 4> lines;
        lines[0] = Vertex { .position = Vector3 { 0, static_cast<float>(m_prev_mouse_pos.y) }, .color = sf::Color::Green };
        lines[1] = Vertex { .position = Vector3 { sizes.x, static_cast<float>(m_prev_mouse_pos.y) }, .color = sf::Color::Green };
        lines[2] = Vertex { .position = Vector3 { static_cast<float>(m_prev_mouse_pos.x), 0 }, .color = sf::Color::Green };
        lines[3] = Vertex { .position = Vector3 { static_cast<float>(m_prev_mouse_pos.x), sizes.y }, .color = sf::Color::Green };
        window.draw_vertices(GL_LINES, lines);
        break;
    }
    default:
        break;
    }

    std::ostringstream oss;
    oss << m_world.date();
    if (m_speed == 0)
        oss << " (Paused";
    else
        oss << " (" << std::abs(m_speed) << "x";

    if (m_speed < 0)
        oss << ", Reversed";
    oss << ")";

    GUI::TextDrawOptions label_text;
    label_text.fill_color = sf::Color::White;
    label_text.font_size = 25;
    window.draw_text("FPS: " + std::to_string(m_fps), GUI::Application::the().font, { 10, window.getSize().y - 65.f });
    window.draw_text(oss.str(), GUI::Application::the().font, { 10, window.getSize().y - 35.f });

    std::ostringstream debugoss;
    auto mp = sf::Mouse::getPosition(window);
    debugoss << "ws=" << screen_to_world({ static_cast<double>(mp.x), static_cast<double>(mp.y), 0 }) << std::endl;
    debugoss << "s=" << scale() << std::endl;
    debugoss << "off=" << offset() << std::endl;
    debugoss << "yaw=" << m_yaw << " $ " << m_yaw_from_object << std::endl;
    debugoss << "pitch=" << m_pitch << " $ " << m_pitch_from_object << std::endl;

    GUI::TextDrawOptions debug_text;
    debug_text.fill_color = sf::Color::White;
    debug_text.font_size = 15;
    window.draw_text(debugoss.str(), GUI::Application::the().fixed_width_font, {600, 20}, debug_text);
}

void SimulationView::pause_simulation(bool state) {
    if (!state) {
        m_speed = m_saved_speed;
    }
    else {
        m_saved_speed = m_speed;
        m_speed = 0;
    }
}

void SimulationView::update() {
    // FIXME: This doesn't quite match here like speed (The same
    //        comment about Simulation object)
    if (m_speed != 0)
        m_world.update(m_speed * m_iterations);

    // Handle focus
    if (m_focused_object) {
        set_offset(-m_focused_object->render_position());

        if (m_focused_object->most_attracting_object() && m_fixed_rotation_on_focus) {
            Vector3 a = m_focused_object->pos() - m_focused_object->most_attracting_object()->pos();

            m_pitch_from_object = std::atan2(a.y, a.z) - M_PI / 2;
            m_yaw_from_object = std::atan2(a.y, a.x) + M_PI / 2;

            if (a.y < 0)
                m_pitch_from_object -= M_PI;
        }
    }

    if (m_focused_object != m_prev_focused_object) {
        if (on_change_focus)
            on_change_focus(m_focused_object);
        m_prev_focused_object = m_focused_object;
    }

    if (m_focused_object != nullptr && if_focused)
        if_focused();
}

Object* SimulationView::focused_object() const {
    if (m_focused_object != nullptr)
        return m_focused_object;
    return {};
}

void SimulationView::setup_python_bindings(TypeSetup type_setup) {
    type_setup.add_method<&SimulationView::python_reset>("reset", "Reset view transform");
    type_setup.add_attribute<&SimulationView::python_get_offset, &SimulationView::python_set_offset>("offset");
    type_setup.add_attribute<&SimulationView::python_get_fov, &SimulationView::python_set_fov>("fov", "Field of view");
    type_setup.add_attribute<&SimulationView::python_get_yaw, &SimulationView::python_set_yaw>("yaw");
    type_setup.add_attribute<&SimulationView::python_get_pitch, &SimulationView::python_set_pitch>("pitch");
    type_setup.add_attribute<&SimulationView::python_get_zoom, &SimulationView::python_set_zoom>("zoom");
    type_setup.add_attribute<&SimulationView::python_get_focused_object, nullptr>("focused_object");
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
    return PySSA::Object::create(m_fov.rad());
}

bool SimulationView::python_set_fov(PySSA::Object const& object) {
    auto v = object.as_double();
    if (!v)
        return false;
    m_fov = Angle(*v, Angle::Unit::Deg);
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

WorldDrawScope const* s_current_draw_scope = nullptr;

void WorldDrawScope::verify() {
    assert(s_current_draw_scope);
}

WorldDrawScope const* WorldDrawScope::current() {
    return s_current_draw_scope;
}

WorldDrawScope::WorldDrawScope(SimulationView const& view, ClearDepth clear_depth, sf::Shader* custom_shader)
    : m_simulation_view(view) {

    if (s_current_draw_scope)
        return;

    auto& shader = custom_shader ? *custom_shader : WorldShader::world_shader();
    view.window().set_shader(&shader);
    apply_uniforms(shader);

    glEnable(GL_DEPTH_TEST);

    if (clear_depth == ClearDepth::Yes)
        glClear(GL_DEPTH_BUFFER_BIT);

    m_parent = s_current_draw_scope;
    s_current_draw_scope = this;
}

void WorldDrawScope::apply_uniforms(sf::Shader& shader) const {
    auto projection_matrix = m_simulation_view.projection_matrix().convert<float>();
    shader.setUniform("projectionMatrix", sf::Glsl::Mat4((float*)projection_matrix.data));
    auto modelview_matrix = m_simulation_view.modelview_matrix().convert<float>();
    shader.setUniform("worldViewMatrix", sf::Glsl::Mat4((float*)modelview_matrix.data));
}

WorldDrawScope::~WorldDrawScope() {
    s_current_draw_scope = m_parent;
    if (s_current_draw_scope)
        return;

    glDisable(GL_DEPTH_TEST);
    m_simulation_view.window().set_shader(nullptr);
}
