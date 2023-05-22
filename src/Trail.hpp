#pragma once

#include "SimulationView.hpp"
#include <Essa/Engine/3D/Shaders/Basic.hpp>
#include <EssaUtil/Vector.hpp>
#include <list>

class Trail {
    struct Vertex : public Essa::Shaders::Basic::Vertex {
        Vertex(Util::Point3f pos, Util::Colorf color)
            : Essa::Shaders::Basic::Vertex(pos, color, {}) { }

        Vertex()
            : Essa::Shaders::Basic::Vertex({}, {}, {}) { }

        auto& position() { return value<0>(); }
        auto& position() const { return value<0>(); }
    };

    std::vector<Vertex> m_vertexes;
    int m_append_offset = 1;
    int m_length = 0;
    Util::Vector3d m_offset;
    bool m_enable_min_step = true;

    double last_xy_angle = 0;

    const Util::Color m_color;

    std::pair<Util::Point3f, Util::Point3f> m_get_last_two_entries() const;

public:
    Trail(size_t max_trail_size, Util::Color color);
    void draw(SimulationView const&) const;
    void push_back(Util::Point3d pos);
    void reset();
    void set_offset(Util::Vector3d offset) { m_offset = offset; }
    void recalculate_with_offset(Util::Vector3d offset);
    void change_current(Util::Point3d pos);
    void set_enable_min_step(bool b) { m_enable_min_step = b; }

private:
    friend std::ostream& operator<<(std::ostream&, Trail const&);
};
