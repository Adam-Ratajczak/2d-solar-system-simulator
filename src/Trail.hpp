#pragma once

#include "SimulationView.hpp"
#include <EssaGUI/glwrapper/Vertex.hpp>
#include <EssaGUI/util/Vector3.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <list>

class Trail {
    std::vector<Vertex> m_vertexes;
    int m_append_offset = 1;
    int m_length = 0;
    Vector3 m_offset;
    bool m_enable_min_step = true;

    double last_xy_angle = 0;

    const sf::Color m_color;

    std::pair<Vector3, Vector3> m_get_last_two_entries() const;

public:
    Trail(size_t max_trail_size, sf::Color color);
    void draw(GUI::SFMLWindow&);
    void push_back(Vector3 pos);
    void reset();
    void set_offset(Vector3 offset) { m_offset = offset; }
    void recalculate_with_offset(Vector3 offset);
    void change_current(Vector3 pos);
    void set_enable_min_step(bool b) { m_enable_min_step = b; }

private:
    friend std::ostream& operator<<(std::ostream&, Trail const&);
};
