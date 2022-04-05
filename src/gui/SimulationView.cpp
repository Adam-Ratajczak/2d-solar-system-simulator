#include "SimulationView.hpp"

#include "../World.hpp"
#include "GUI.hpp"
#include <cmath>
#include <sstream>

void SimulationView::start_focus_measure() {
    if (m_focused_object != nullptr) {
        m_focused_object->m_focused = false;
        m_focused_object = nullptr;
    }
    m_focus_measure = true;
}

void SimulationView::handle_event(Event& event) {
    m_changed = false;
    if (event.type() == sf::Event::MouseButtonPressed) {
        m_prev_mouse_pos = { static_cast<float>(event.event().mouseButton.x), static_cast<float>(event.event().mouseButton.y) };
        if (event.event().mouseButton.button == sf::Mouse::Left) {
            auto world_click_pos = screen_to_world({ static_cast<float>(event.event().mouseButton.x), static_cast<float>(event.event().mouseButton.y) });
            m_prev_pos = world_click_pos;
            m_dragging = true;
            // clang-format off

            m_world.for_each_object([&](Object& object) {
                if(object.hover(*this, world_click_pos))
                {
                    if(m_focused_object != nullptr)
                        m_focused_object->m_focused = false;
                    m_focused_object = &object;
                    m_focused_object->m_focused = true;
                }
            });
            // clang-format on
            if (m_coord_measure) {
                m_coord_measure = false;
                m_measured = true;
                if (on_coord_measure) {
                    on_coord_measure(m_prev_pos);
                }
            }
            else if (m_focus_measure && m_focused_object != nullptr) {
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
            if (m_focused_object) {
                m_focused_object->m_focused = false;
                m_focused_object = nullptr;
            }

            m_coord_measure = false;
            m_focus_measure = false;
        }
    }
    else if (event.type() == sf::Event::MouseWheelScrolled) {
        if (event.event().mouseWheelScroll.delta > 0)
            apply_zoom(1.1);
        else
            apply_zoom(1 / 1.1);
        m_changed = true;
    }
    else if (event.type() == sf::Event::MouseMoved) {
        m_prev_mouse_pos = { static_cast<float>(event.event().mouseMove.x), static_cast<float>(event.event().mouseMove.y)};
        if (m_dragging) {
            const sf::Vector2f newPos = screen_to_world(m_prev_mouse_pos);
            const sf::Vector2f deltaPos = m_prev_pos - newPos;
            set_offset(offset() + deltaPos);
            // std::cout << "mouse_pos: " << mouse_pos << " view offset: " << view.offset() << " prev_pos: " << prev_pos << " delta_pos: " << deltaPos << std::endl;
        }
    }
    else if (event.type() == sf::Event::MouseButtonReleased) {
        m_dragging = false;
    }
    else if (event.type() == sf::Event::KeyPressed) {
        // FIXME: This is too complex and doesn't even work for all cases.
        if (event.event().key.code == sf::Keyboard::Right) {
            if (m_speed > 0)
                m_speed *= 2;
            else {
                if (m_speed == 0)
                    m_speed = 1;
                else
                    m_speed /= 2;
            }
        }
        else if (event.event().key.code == sf::Keyboard::Left) {
            if (m_speed < 0)
                m_speed *= 2;
            else {
                if (m_speed == 0)
                    m_speed = -1;
                else
                    m_speed /= 2;
            }
        }
    }
    m_changed = m_dragging;
}

void SimulationView::draw_grid(sf::RenderWindow& window) const {
    constexpr float zoom_step_exponent = 2;
    auto zoom_rounded_to_power = std::pow(zoom_step_exponent, std::round(std::log2(m_zoom) / std::log2(zoom_step_exponent)));
    float spacing = AU / zoom_rounded_to_power / 10;
    constexpr int major_gridline_interval = 4;
    sf::Vector2f start_coords = screen_to_world({ 0, 0 });
    start_coords.x -= std::remainder(start_coords.x, spacing * major_gridline_interval) + spacing;
    start_coords.y -= std::remainder(start_coords.y, spacing * major_gridline_interval) + spacing;
    sf::Vector2f end_coords = screen_to_world({ size().x, size().y });

    sf::Color const major_grid_line_color { 60, 60, 60 };
    sf::Color const grid_line_color { 25, 25, 25 };

    sf::VertexArray lines(sf::Lines);
    int index = 0;
    for (float x = start_coords.x; x < end_coords.x; x += spacing) {
        auto color = index % major_gridline_interval == 1 ? major_grid_line_color : grid_line_color;
        lines.append(sf::Vertex { { static_cast<float>(world_to_screen({ x, 0 }).x), 0 }, color });
        lines.append(sf::Vertex { { static_cast<float>(world_to_screen({ x, 0 }).x), size().y }, color });
        index++;
    }
    index = 0;
    for (float y = start_coords.y; y < end_coords.y; y += spacing) {
        auto color = index % major_gridline_interval == 1 ? major_grid_line_color : grid_line_color;
        lines.append(sf::Vertex { { 0, static_cast<float>(world_to_screen({ 0, y }).y) }, color });
        lines.append(sf::Vertex { { size().x, static_cast<float>(world_to_screen({ 0, y }).y) }, color });
        index++;
    }
    window.draw(lines);

    // guide
    sf::Vector2f guide_start { size().x - 200.f, size().y - 30.f };
    sf::Vector2f guide_end = guide_start - sf::Vector2f(spacing * scale(), 0);
    sf::VertexArray guide { sf::Lines, 6 };
    sf::Color const guide_color { 127, 127, 127 };
    guide[0] = sf::Vertex({ guide_start, guide_color });
    guide[1] = sf::Vertex({ guide_end, guide_color });
    guide[2] = sf::Vertex({ guide_start - sf::Vector2(0, 5), guide_color });
    guide[3] = sf::Vertex({ guide_start + sf::Vector2(0, 5), guide_color });
    guide[4] = sf::Vertex({ guide_end - sf::Vector2(0, 5), guide_color });
    guide[5] = sf::Vertex({ guide_end + sf::Vector2(0, 5), guide_color });
    window.draw(guide);

    sf::Text text { std::to_string((size_t)spacing / 1000) + " km", GUI::font, 15 };
    text.setPosition(guide_start);
    auto bounds = text.getLocalBounds();
    text.setOrigin({ bounds.width, bounds.height + 10 });
    window.draw(text);
}

void SimulationView::draw(sf::RenderWindow& window) const {
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
    oss << m_world.date.to_string();
    if (m_speed == 0)
        oss << " (Paused";
    else
        oss << " (" << std::abs(m_speed) << "x";

    if (m_speed < 0)
        oss << ", Reversed";
    oss << ")";

    sf::Text fps_text("FPS: " + std::to_string(m_fps), GUI::font, 25);
    fps_text.setFillColor(sf::Color::White);
    fps_text.setPosition(10, window.getSize().y - 65);
    window.draw(fps_text);

    sf::Text date_text(oss.str(), GUI::font, 25);
    date_text.setFillColor(sf::Color::White);
    date_text.setPosition(10, window.getSize().y - 35);
    window.draw(date_text);
}

void SimulationView::update() {
    // FIXME: This doesn't quite match here like speed (The same
    //        comment about Simulation object)
    if (m_speed != 0)
        m_world.update(m_speed * m_iterations);

    // Handle focus
    if (m_focused_object)
        set_offset(m_focused_object->m_pos);
}
