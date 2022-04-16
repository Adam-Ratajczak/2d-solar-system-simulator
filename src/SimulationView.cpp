#include "SimulationView.hpp"

#include "World.hpp"
#include "glwrapper/Helpers.hpp"
#include "gui/Application.hpp"
#include "math/Ray.hpp"
#include "math/Transform.hpp"

#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <optional>
#include <sstream>

void SimulationView::start_focus_measure() {
    if (m_focused_object != nullptr) {
        m_focused_object->m_focused = false;
        m_focused_object = nullptr;
    }
    m_focus_measure = true;
}

void SimulationView::handle_event(GUI::Event& event) {
    if (event.type() == sf::Event::MouseButtonPressed) {
        m_prev_mouse_pos = { static_cast<float>(event.event().mouseButton.x), static_cast<float>(event.event().mouseButton.y) };
        m_prev_drag_pos = m_prev_mouse_pos;
        if (event.event().mouseButton.button == sf::Mouse::Left) {
            m_drag_mode = DragMode::Pan;

            m_world.for_each_object([&](Object& obj) {
                auto obj_pos_screen = world_to_screen(obj.m_pos / AU);
                obj_pos_screen.z = 0;
                auto distance = obj_pos_screen.distance_to(m_prev_mouse_pos);
                if (distance < 30)
                    m_focused_object = &obj;
            });

            if (m_coord_measure)
                m_coord_measure = false;
            if (m_focus_measure && m_focused_object != nullptr) {
                m_focus_measure = false;
                m_measured = true;
                if (on_focus_measure) {
                    on_focus_measure(m_focused_object);
                }
                m_focused_object->m_focused = false;
                m_focused_object = nullptr;
            }
            event.set_handled();
        }
        else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            m_coord_measure = false;
            m_focus_measure = false;

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

        // DRAG
        auto drag_delta = m_prev_drag_pos - mouse_pos;

        if (m_drag_mode != DragMode::None && !m_is_dragging && (std::abs(drag_delta.x) > 20 || std::abs(drag_delta.y) > 20)) {
            m_is_dragging = true;

            if (m_drag_mode == DragMode::Pan && m_focused_object) {
                m_focused_object->m_focused = false;
                m_focused_object = nullptr;
            }
        }

        if (m_is_dragging) {
            switch (m_drag_mode) {
            case DragMode::Pan: {
                Vector3 qad_delta { drag_delta.x, -drag_delta.y, 0 };
                set_offset(offset() + (Transform::rotation_around_z(-m_yaw) * qad_delta * scale() / 320));
                break;
            }
            case DragMode::Rotate: {
                auto sizes = window().getSize();
                m_yaw += drag_delta.x / sizes.x * M_PI;
                m_pitch += drag_delta.y / sizes.y * M_PI;
                break;
            }
            default:
                break;
            }
            m_prev_drag_pos = mouse_pos;
        }

        // COORD MEASURE
        if (m_coord_measure) {
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
                if (on_coord_measure) {
                    on_coord_measure(object_pos_in_world_space * AU);
                }
            }
        }
    }
    else if (event.type() == sf::Event::MouseButtonReleased) {
        m_is_dragging = false;
        m_drag_mode = DragMode::None;
    }
    else if (event.type() == sf::Event::KeyPressed) {
        // FIXME: This is too complex and doesn't even work for all cases.
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

void SimulationView::draw_grid(sf::RenderWindow& window) const {
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

        GL::draw_vertices(GL_LINES, vertices);
    }

    // guide
    sf::Vector2f guide_start { size().x - 200.f, size().y - 30.f };
    // HACK: this *100 should be calculated from perspective somehow
    sf::Vector2f guide_end = guide_start - sf::Vector2f(spacing * scale() * 100, 0);
    sf::VertexArray guide { sf::Lines, 6 };
    sf::Color const guide_color { 127, 127, 127 };
    guide[0] = sf::Vertex({ guide_start, guide_color });
    guide[1] = sf::Vertex({ guide_end, guide_color });
    guide[2] = sf::Vertex({ guide_start - sf::Vector2f(0, 5), guide_color });
    guide[3] = sf::Vertex({ guide_start + sf::Vector2f(0, 5), guide_color });
    guide[4] = sf::Vertex({ guide_end - sf::Vector2f(0, 5), guide_color });
    guide[5] = sf::Vertex({ guide_end + sf::Vector2f(0, 5), guide_color });
    window.draw(guide);

    // FIXME: UB on size_t conversion
    sf::Text text { std::to_string((size_t)(spacing * AU) / 1000) + " km", application().font, 15 };
    text.setPosition(guide_start);
    auto bounds = text.getLocalBounds();
    text.setOrigin({ bounds.width, bounds.height + 10 });
    window.draw(text);
}

Matrix4x4d SimulationView::projection_matrix() const {
    double fov = 80;
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
    matrix = matrix * Transform::rotation_around_z(m_yaw);
    matrix = matrix * Transform::rotation_around_x(m_pitch);
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

void SimulationView::draw(sf::RenderWindow& window) const {
    if (m_show_grid)
        draw_grid(window);
    m_world.draw(*this);

    if (m_coord_measure) {
        auto sizes = size();
        sf::VertexArray lines(sf::Lines, 4);
        lines[0] = sf::Vertex { { 0, static_cast<float>(m_prev_mouse_pos.y) }, sf::Color::Red };
        lines[1] = sf::Vertex { { sizes.x, static_cast<float>(m_prev_mouse_pos.y) }, sf::Color::Red };
        lines[2] = sf::Vertex { { static_cast<float>(m_prev_mouse_pos.x), 0 }, sf::Color::Red };
        lines[3] = sf::Vertex { { static_cast<float>(m_prev_mouse_pos.x), sizes.y }, sf::Color::Red };
        window.draw(lines);
    }
    else if (m_focus_measure) {
        auto sizes = size();
        sf::VertexArray lines(sf::Lines, 4);
        lines[0] = sf::Vertex { { 0, static_cast<float>(m_prev_mouse_pos.y) }, sf::Color::Green };
        lines[1] = sf::Vertex { { sizes.x, static_cast<float>(m_prev_mouse_pos.y) }, sf::Color::Green };
        lines[2] = sf::Vertex { { static_cast<float>(m_prev_mouse_pos.x), 0 }, sf::Color::Green };
        lines[3] = sf::Vertex { { static_cast<float>(m_prev_mouse_pos.x), sizes.y }, sf::Color::Green };
        window.draw(lines);
    }

    std::ostringstream oss;
    oss << m_world.date().to_string();
    if (m_speed == 0)
        oss << " (Paused";
    else
        oss << " (" << std::abs(m_speed) << "x";

    if (m_speed < 0)
        oss << ", Reversed";
    oss << ")";

    sf::Text fps_text("FPS: " + std::to_string(m_fps), application().font, 25);
    fps_text.setFillColor(sf::Color::White);
    fps_text.setPosition(10, window.getSize().y - 65);
    window.draw(fps_text);

    sf::Text date_text(oss.str(), application().font, 25);
    date_text.setFillColor(sf::Color::White);
    date_text.setPosition(10, window.getSize().y - 35);
    window.draw(date_text);

    std::ostringstream debugoss;
    auto mp = sf::Mouse::getPosition(window);
    debugoss << "ws=" << screen_to_world({ static_cast<double>(mp.x), static_cast<double>(mp.y), 0 }) << std::endl;
    debugoss << "s=" << scale() << std::endl;
    debugoss << "off=" << offset() << std::endl;

    sf::Text debug_text(debugoss.str(), application().font, 15);
    debug_text.setPosition(600, 10);
    window.draw(debug_text);
}

void SimulationView::update() {
    // FIXME: This doesn't quite match here like speed (The same
    //        comment about Simulation object)
    if (m_speed != 0)
        m_world.update(m_speed * m_iterations);

    // Handle focus
    if (m_focused_object)
        set_offset(-m_focused_object->m_pos / AU);
}

Object* SimulationView::focused_object() const{
    if(m_focused_object != nullptr)
        return m_focused_object;
    return {};
}

static size_t s_world_draw_scope_recursion = 0;

void WorldDrawScope::verify() {
    assert(s_world_draw_scope_recursion > 0);
}

WorldDrawScope::WorldDrawScope(SimulationView const& view, ClearDepth clear_depth)
    : m_simulation_view(view) {
    s_world_draw_scope_recursion++;
    if (s_world_draw_scope_recursion > 1)
        return;

    view.window().popGLStates();

    glViewport(0, 0, view.size().x, view.size().y);

    // simple OpenGL test
    glMatrixMode(GL_PROJECTION);
    view.projection_matrix().gl_load();

    glMatrixMode(GL_MODELVIEW);
    view.modelview_matrix().gl_load();

    glEnable(GL_DEPTH_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    if (clear_depth == ClearDepth::Yes)
        glClear(GL_DEPTH_BUFFER_BIT);
}

WorldDrawScope::~WorldDrawScope() {
    s_world_draw_scope_recursion--;
    if (s_world_draw_scope_recursion != 0)
        return;
    glFlush();
    m_simulation_view.window().pushGLStates();
    m_simulation_view.window().resetGLStates();
}
