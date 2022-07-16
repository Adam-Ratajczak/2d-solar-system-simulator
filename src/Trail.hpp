#pragma once

#include "SimulationView.hpp"
#include <EssaUtil/Vector.hpp>
#include <list>

class Trail {
    std::vector<llgl::Vertex> m_vertexes;
    int m_append_offset = 1;
    int m_length = 0;
    Util::Vector3d m_offset;
    bool m_enable_min_step = true;

    double last_xy_angle = 0;

    const Util::Color m_color;

    std::pair<Util::Vector3f, Util::Vector3f> m_get_last_two_entries() const;

public:
    Trail(size_t max_trail_size, Util::Color color);
    void draw(GUI::Window&);
    void push_back(Util::Vector3d pos);
    void reset();
    void set_offset(Util::Vector3d offset) { m_offset = offset; }
    void recalculate_with_offset(Util::Vector3d offset);
    void change_current(Util::Vector3d pos);
    void set_enable_min_step(bool b) { m_enable_min_step = b; }

private:
    friend std::ostream& operator<<(std::ostream&, Trail const&);
};
