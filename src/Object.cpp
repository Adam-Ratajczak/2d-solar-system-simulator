#include "Object.hpp"

#include "SimulationView.hpp"
#include "World.hpp"
#include "WorldShader.hpp"
#include "glwrapper/Sphere.hpp"
#include "math/Transform.hpp"
#include "pyssa/Object.hpp"
#include "pyssa/TupleParser.hpp"
#include <EssaGUI/gfx/SFMLWindow.hpp>
#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/util/DelayedInit.hpp>
#include <EssaGUI/util/UnitDisplay.hpp>
#include <EssaGUI/util/Units.hpp>

#include <GL/gl.h>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

static Util::DelayedInit<Sphere> s_sphere;

Object::Object(double mass, double radius, Vector3 pos, Vector3 vel, sf::Color color, std::string name, unsigned period)
    : m_trail(std::max(2U, std::max(period * 2, (unsigned)500)), color)
    // FIXME: Share the sphere as it is identical for all objects and
    //        takes most of the object's used memory.
    , m_history(1000, { pos, vel })
    , m_gravity_factor(mass * G)
    , m_radius(radius)
    , m_pos(pos)
    , m_vel(vel)
    , m_color(color)
    , m_name(name)
    , m_orbit_len(period) {
    m_trail.push_back(m_pos);
}

Sphere& Object::sphere() {
    if (!s_sphere.is_initialized()) {
        s_sphere.construct();
    }

    return *s_sphere;
}

Vector3 Object::attraction(const Object& other) {
    Vector3 dist = this->m_pos - other.m_pos;
    double force = other.m_gravity_factor / dist.magnitude_squared();
    Vector3 normalized_dist = dist.normalized();
    return normalized_dist * force;
}

bool Object::deleted() const {
    return (m_deleted && m_deletion_date <= m_world->date()) || m_creation_date > m_world->date();
}

void Object::update_forces_against(Object& object) {
    // TODO: Collisions
    Vector3 dist = this->m_pos - object.m_pos;
    // denominator: R^2*normalized(dist)
    double denominator = dist.magnitude_squared();
    denominator *= std::sqrt(denominator);
    auto attraction_base = dist / denominator;

    auto calculate_attraction = [&](Vector3& this_attraction, Vector3& other_attraction) mutable {
        this_attraction = attraction_base * object.m_gravity_factor;
        m_attraction_factor -= this_attraction;

        other_attraction = attraction_base * m_gravity_factor;
        object.m_attraction_factor += other_attraction;
    };

    Vector3 this_attraction, other_attraction;

    calculate_attraction(this_attraction, other_attraction);

    auto this_attraction_mag = this_attraction.magnitude_squared() / object.m_gravity_factor;
    if (this_attraction_mag > m_max_attraction && object.m_gravity_factor > m_gravity_factor) {
        m_max_attraction = this_attraction_mag;
        m_most_attracting_object = &object;
    }

    auto other_attraction_mag = other_attraction.magnitude_squared() / m_gravity_factor;
    if (other_attraction_mag > object.m_max_attraction && object.m_gravity_factor < m_gravity_factor) {
        object.m_max_attraction = other_attraction_mag;
        object.m_most_attracting_object = this;
    }
}

void Object::before_update() {
    if (m_is_forward_simulated)
        update_closest_approaches();
    m_old_most_attracting_object = m_most_attracting_object;
}

void Object::nonphysical_update() {
    if (m_world->m_simulation_view->offset_trails())
        recalculate_trails_with_offset();
    else {
        m_trail.recalculate_with_offset({});
        m_trail.push_back(m_pos);
    }

    if (m_most_attracting_object == nullptr)
        return;

    double distance_from_object = get_distance(this->m_pos, m_most_attracting_object->m_pos);
    if (m_ap < distance_from_object) {
        m_ap = distance_from_object;
        m_ap_vel = m_vel.magnitude();
    }

    if (m_pe > distance_from_object) {
        m_pe = distance_from_object;
        m_pe_vel = m_vel.magnitude();
    }
}

void Object::clear_forces() {
    m_attraction_factor = Vector3();
    m_max_attraction = 0;
}

void Object::update(int speed) {
    if (speed > 0) {
        if (!m_is_forward_simulated) {
            auto val = m_history.move_forward({ m_pos, m_vel });

            if (val.has_value()) {
                m_pos = val.value().pos;
                m_vel = val.value().vel;
            }
        }
    }
    else if (speed < 0) {
        assert(!m_is_forward_simulated);
        auto val = m_history.move_backward({ m_pos, m_vel });

        if (val.has_value()) {
            m_pos = val.value().pos;
            m_vel = val.value().vel;
        }
    }

    nonphysical_update();
}

void Object::recalculate_trails_with_offset() {
    if (!m_most_attracting_object || m_is_forward_simulated) {
        m_trail.recalculate_with_offset({});
        m_trail.push_back(m_pos);
        return;
    }

    if (m_most_attracting_object != m_old_most_attracting_object)
        m_trail.recalculate_with_offset(m_most_attracting_object->m_pos);
    else
        m_trail.set_offset(m_most_attracting_object->m_pos);
    m_trail.push_back(m_pos - m_most_attracting_object->m_pos);
}

void Object::delete_most_attracting_object() {
    m_most_attracting_object = nullptr;
    m_trail.recalculate_with_offset({});
    m_trail.reset();
}

void Object::draw(SimulationView const& view) {
    WorldDrawScope::verify();

    auto scaled_pos = render_position();
    auto& target = view.window();

    s_sphere->set_radius(m_radius / AU);
    s_sphere->set_position(scaled_pos);
    s_sphere->set_color(m_color);
    s_sphere->set_light_position(m_world->light_source() ? m_world->light_source()->pos() / AU : Vector3());
    s_sphere->draw(view.window());

    if (view.show_trails())
        m_trail.draw(view.window());
}

void Object::draw_closest_approaches(SimulationView const& view) {
    WorldDrawScope::verify();

    std::vector<Vertex> closest_approaches_vertexes;
    for (auto& closest_approach_entry : m_closest_approaches) {
        if (closest_approach_entry.second.distance > AU / 10)
            continue;
        closest_approaches_vertexes.push_back(Vertex { .position = closest_approach_entry.second.this_position / AU, .color = sf::Color(m_color.r, m_color.g, m_color.b, 100) });
        sf::Color other_color { closest_approach_entry.first->m_color.r, closest_approach_entry.first->m_color.g, closest_approach_entry.first->m_color.b, 100 };
        closest_approaches_vertexes.push_back(Vertex { .position = closest_approach_entry.second.other_object_position / AU, .color = other_color });
    }
    view.window().draw_vertices(GL_LINES, closest_approaches_vertexes);
}

void Object::draw_closest_approaches_gui(SimulationView const& view) {
    for (auto& closest_approach_entry : m_closest_approaches) {
        if (closest_approach_entry.second.distance > AU / 10)
            continue;
        auto position = (closest_approach_entry.second.this_position + closest_approach_entry.second.other_object_position) / (2 * AU);
        std::ostringstream oss;
        oss << "CA with " << closest_approach_entry.first->name() << ": " << Util::unit_display(closest_approach_entry.second.distance, Util::Quantity::Length).to_string().toAnsiString();
        draw_label(view, position, oss.str(), closest_approach_entry.first->m_color);
    }
}

void Object::draw_label(SimulationView const& sv, Vector3 position, std::string string, sf::Color color) const {
    auto screen_position = sv.world_to_screen(position);

    // Don't draw labels of planets outside of clipping box
    if (screen_position.z > 1 || screen_position.z < -1)
        return;

    GUI::TextDrawOptions text;
    text.font_size = 15;
    text.fill_color = color;
    sv.window().draw_text(string, GUI::Application::the().bold_font, { std::roundf(screen_position.x), std::roundf(screen_position.y) }, text);
}

void Object::delete_object() {
    m_deletion_date = m_world->date();
    m_deleted = true;
}

Object::Info Object::get_info() const {
    Info info {
        .mass = mass(),
        .radius = m_radius,
        .absolute_velocity = m_vel.magnitude()
    };

    if (m_most_attracting_object) {
        info.distance_from_most_massive_object = get_distance(this->m_pos, m_most_attracting_object->m_pos);
        info.apogee = m_ap;
        info.apogee_velocity = m_ap_vel;
        info.perigee = m_pe;
        info.perigee_velocity = m_pe_vel;
        info.orbit_period = m_orbit_len;
        info.orbit_eccencrity = eccencrity;
    }

    return info;
}

void Object::reset_history() {
    m_history.reset();
    m_trail.reset();
}

void Object::draw_gui(SimulationView const& view) {

    if (!view.show_labels())
        return;
    draw_label(view, render_position(), m_name, m_is_forward_simulated ? sf::Color(128, 128, 128) : sf::Color::White);
}

std::unique_ptr<Object> Object::create_object_relative_to_ap_pe(double mass, Distance radius, Distance apogee, Distance perigee, bool direction, Angle theta, Angle alpha, sf::Color color, std::string name, Angle rotation) {
    // formulae used from site: https://www.scirp.org/html/6-9701522_18001.htm
    // std::cout << m_gravity_factor << "\n";
    double GM = m_gravity_factor;
    double a = (apogee.value() + perigee.value()) / 2;
    double b = std::sqrt(apogee.value() * perigee.value());

    double T = 2 * M_PI * std::sqrt((a * a * a) / GM);
    // T = T * std::sin(theta.rad()) + T * std::cos(alpha.rad());
    Vector3 pos(std::cos(theta.rad()) * std::cos(alpha.rad()) * a, std::sin(theta.rad()) * std::cos(alpha.rad()) * a, std::sin(alpha.rad()) * a);
    pos = pos.rotate_vector(rotation.rad());
    pos += this->m_pos;

    auto result = std::make_unique<Object>(mass, radius.value(), pos, Vector3(0, 0), color, name, T / (3600 * 24));
    result->m_ap = apogee.value();
    result->m_pe = perigee.value();

    result->eccencrity = std::sqrt(1 - (b * b) / (a * a));
    // std::cout << result.eccencrity << "\n";

    double velocity_constant = (2 * GM) / (apogee.value() + perigee.value());

    result->m_ap_vel = std::sqrt(2 * GM / apogee.value() - velocity_constant);
    result->m_pe_vel = std::sqrt(2 * GM / perigee.value() - velocity_constant);
    double velocity = std::sqrt(2 * GM / get_distance(pos, this->m_pos) - velocity_constant);

    Vector3 vel(std::cos(theta.rad() + M_PI / 2) * velocity, std::sin(theta.rad() + M_PI / 2) * velocity);

    if (direction)
        vel = -vel;

    vel += this->m_vel;
    result->m_vel = vel;

    return result;
}
std::unique_ptr<Object> create_object_relative_to_maj_ecc(double mass, Distance radius, Distance semi_major, double ecc, bool direction, Angle theta, Angle alpha, sf::Color color, std::string name, Angle rotation);

void Object::add_object_relative_to(double mass, Distance radius, Distance apogee, Distance perigee, bool direction, Angle theta, Angle alpha, sf::Color color, std::string name, Angle rotation) {
    m_world->add_object(create_object_relative_to_ap_pe(mass, radius, apogee, perigee, direction, theta, alpha, color, name, rotation));
}

std::unique_ptr<Object> Object::create_object_relative_to_maj_ecc(double mass, Distance radius, Distance semi_major, double ecc, bool direction, Angle theta, Angle alpha, sf::Color color, std::string name, Angle rotation) {

    double GM = m_gravity_factor;
    double a = semi_major.value();

    double T = 2 * M_PI * std::sqrt((a * a * a) / GM);
    // T = T * std::sin(theta.rad()) + T * std::cos(alpha.rad());
    Vector3 pos(std::cos(theta.rad()) * std::cos(alpha.rad()) * a, std::sin(theta.rad()) * std::cos(alpha.rad()) * a, std::sin(alpha.rad()) * a);
    pos = pos.rotate_vector(rotation.rad());
    pos += this->m_pos;

    auto result = std::make_unique<Object>(mass, radius.value(), pos, Vector3(0, 0), color, name, T / (3600 * 24));
    result->m_ap = 0;
    result->m_pe = std::numeric_limits<double>::max();

    result->eccencrity = ecc;
    // std::cout << result.eccencrity << "\n";

    double velocity_constant = (2 * GM) / (a * 2 - a * ecc);

    result->m_ap_vel = std::sqrt(2 * GM / a - velocity_constant);
    result->m_pe_vel = std::sqrt(2 * GM / (a - a * ecc) - velocity_constant);
    double velocity = std::sqrt(2 * GM / get_distance(pos, this->m_pos) - velocity_constant);

    Vector3 vel(std::cos(theta.rad() + M_PI / 2) * velocity, std::sin(theta.rad() + M_PI / 2) * velocity);

    if (direction)
        vel = -vel;

    vel += this->m_vel;
    result->m_vel = vel;

    return result;
}

std::unique_ptr<Object> Object::clone_for_forward_simulation() const {
    auto brightened_color = [](sf::Color const& color) {
        return sf::Color {
            (uint8_t)std::min(255, color.r + 60),
            (uint8_t)std::min(255, color.g + 60),
            (uint8_t)std::min(255, color.b + 60),
            255
        };
    };
    auto object = std::make_unique<Object>(mass() * 1000, m_radius, m_pos, m_vel, brightened_color(m_color), m_name, 500);
    object->m_is_forward_simulated = true;
    object->trail().set_enable_min_step(false);
    return object;
}

void Object::require_orbit_point(Vector3 pos) {
    std::cout << "TODO: require_orbit_point " << pos << std::endl;
}

void Object::update_closest_approaches() {
    m_world->for_each_object([this](Object& object) {
        if (&object == this)
            return;
        auto& closest_approach_entry = m_closest_approaches[&object];
        auto distance = object.m_pos.distance_to(m_pos);
        if (closest_approach_entry.distance == 0 || distance < closest_approach_entry.distance) {
            closest_approach_entry.distance = distance;
            closest_approach_entry.this_position = m_pos;
            closest_approach_entry.other_object_position = object.m_pos;
        }
    });
}

void Object::set_radius(double radius) {
    m_radius = radius;
}

std::ostream& operator<<(std::ostream& out, Object const& object) {
    return out << "Object(" << object.m_name << ") @ " << object.m_pos;
}

void Object::setup_python_bindings(TypeSetup setup) {
    setup.add_method<&Object::python_attraction>("attraction", "Returns gravity of this object to object given as argument, as acceleration (vector, in m/tick^2)");
    setup.add_attribute<&Object::python_get_pos, &Object::python_set_pos>("pos", "Object position (vector, in m)");
    setup.add_attribute<&Object::python_get_vel, &Object::python_set_vel>("vel", "Object velocity (vector, in m/tick)");
    setup.add_attribute<&Object::python_get_name, &Object::python_set_name>("name", "Object name (to be used with world.get_object_by_name())");
    setup.add_attribute<&Object::python_get_color, &Object::python_set_color>("color", "Object sphere and trail color (RGB)");
    setup.add_attribute<&Object::python_get_radius, &Object::python_set_radius>("radius", "Object radius (in meters)");
}

Object* Object::create_for_python(PySSA::Object const& args, PySSA::Object const& kwargs) {
    PyObject* world = nullptr;
    double mass = 0;
    double radius = 0;
    Vector3 pos;
    Vector3 vel;
    sf::Color color;
    char const* name = "Planet";
    unsigned period = 1;

    static char const* keywords[] = {
        "mass",
        "radius",
        "pos",
        "vel",
        "color",
        "name",
        "period",
        nullptr
    };

    if (!PyArg_ParseTupleAndKeywords(args.python_object(), kwargs.python_object(), "|$dd(ddd)(ddd)(bbb)su", (char**)keywords,
            &mass,
            &radius,
            &pos.x, &pos.y, &pos.z,
            &vel.x, &vel.y, &vel.z,
            &color.r, &color.g, &color.b,
            &name,
            &period))
        return {};

    return new Object(mass, radius, pos, vel, color, name, period);
}

PySSA::Object Object::python_attraction(PySSA::Object const& args, PySSA::Object const& kwargs) {
    Object* other {};
    if (!PySSA::parse_arguments(args, kwargs, "O!", PySSA::Arg::Arg { PySSA::Arg::CheckedType<Object> { other }, "other" }))
        return {};
    return PySSA::Object::create(attraction(*other));
}

PySSA::Object Object::python_get_pos() const {
    return PySSA::Object::create(m_pos);
}

bool Object::python_set_pos(PySSA::Object const& value) {
    auto maybe_value = value.as_vector();
    if (!maybe_value.has_value())
        return false;
    m_pos = maybe_value.value();
    return true;
}

PySSA::Object Object::python_get_vel() const {
    return PySSA::Object::create(m_vel);
}

bool Object::python_set_vel(PySSA::Object const& value) {
    auto maybe_value = value.as_vector();
    if (!maybe_value.has_value())
        return false;
    m_vel = maybe_value.value();
    return true;
}

PySSA::Object Object::python_get_name() const {
    return PySSA::Object::create(m_name);
}

bool Object::python_set_name(PySSA::Object const& value) {
    auto maybe_value = value.as_string();
    if (!maybe_value.has_value())
        return false;
    m_name = maybe_value.value();
    return true;
}

PySSA::Object Object::python_get_color() const {
    return PySSA::Object::create(m_color);
}

bool Object::python_set_color(PySSA::Object const& value) {
    auto maybe_value = value.as_color();
    if (!maybe_value.has_value())
        return false;
    m_color = maybe_value.value();
    return true;
}

PySSA::Object Object::python_get_radius() const {
    return PySSA::Object::create(m_radius);
}

bool Object::python_set_radius(PySSA::Object const& value) {
    auto maybe_value = value.as_double();
    if (!maybe_value.has_value())
        return false;
    m_radius = maybe_value.value();
    return true;
}
