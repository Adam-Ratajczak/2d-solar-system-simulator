#include "Trail.hpp"

#include "Object.hpp"
#include "glwrapper/Helpers.hpp"

#include <Essa/Engine/3D/Shaders/Basic.hpp>
#include <Essa/LLGL/Core/Transform.hpp>
#include <Essa/LLGL/OpenGL/PrimitiveType.hpp>
#include <Essa/LLGL/OpenGL/Vertex.hpp>
#include <Essa/LLGL/OpenGL/VertexArray.hpp>
#include <EssaUtil/DelayedInit.hpp>
#include <EssaUtil/Vector.hpp>
#include <cmath>
#include <iostream>

static constexpr auto TrailMinStep = 60 * 60 * 12;
static constexpr size_t TrailMaxAllocatedSpace = 0xfffff;

Trail::Trail(size_t max_trail_size, Util::Color color)
    : m_color(color) {

    // Trail must have at least the beginning, the end and the "glue" vertex
    if (max_trail_size < 3) {
        std::cout << "FIXME: Trail: Something is wrong! Trying to create trail of size < 3" << std::endl;
        max_trail_size = 3;
    }

    if (max_trail_size > TrailMaxAllocatedSpace) {
        std::cout << "FIXME: Trail: Something is wrong! Trying to create trail of size " << max_trail_size << ", max is " << TrailMaxAllocatedSpace << std::endl;
        max_trail_size = TrailMaxAllocatedSpace;
    }
    m_vertexes.resize(max_trail_size);
    m_length++;
}

std::pair<Util::Vector3f, Util::Vector3f> Trail::m_get_last_two_entries() const {
    int i1 = m_append_offset;
    int i2 = m_append_offset - 1;

    if (i1 < 0)
        i1 += m_vertexes.size();

    if (i2 < 0)
        i2 += m_vertexes.size();

    return std::make_pair(m_vertexes[i1].value<0>(), m_vertexes[i2].value<0>());
}

void Trail::push_back(Util::Vector3d pos) {
    // Ensure that the trail always has the beginning
    if (m_length == 1) {
        assert(m_append_offset == 1);
        m_vertexes[m_append_offset] = Vertex { Util::Vector3f { pos / Util::Constants::AU }, m_color };
        m_append_offset++;
        m_length++;
    }

    if (m_length >= 3 && m_append_offset != 0) {
        if (std::fabs(std::atan2(pos.y(), pos.x()) - last_xy_angle) <= 2 * M_PI / m_vertexes.size()) {
            change_current(pos);
            return;
        }
    }
    m_vertexes[m_append_offset] = Vertex { Util::Vector3f { pos / Util::Constants::AU }, m_color };

    m_append_offset++;
    if (static_cast<size_t>(m_length) < m_vertexes.size())
        m_length++;
    if (static_cast<size_t>(m_append_offset) == m_vertexes.size()) {
        m_vertexes[0] = Vertex { Util::Vector3f { pos / Util::Constants::AU }, m_color };
        m_append_offset = 1;
    }

    last_xy_angle = std::atan2(pos.y(), pos.x());
}

void Trail::reset() {
    m_append_offset = 1;
    m_length = 1;
}

void Trail::recalculate_with_offset(Util::Vector3d offset) {
    if (m_offset == offset)
        return;
    for (int s = 0; s < m_length; s++)
        m_vertexes[s].value<0>() = m_vertexes[s].value<0>() + Util::Vector3f { m_offset / Util::Constants::AU - offset / Util::Constants::AU };
    m_offset = offset;
}

void Trail::draw(SimulationView const& sv) const {
    static Essa::Shaders::Basic shader;
    Essa::Shaders::Basic::Uniforms uniforms;
    uniforms.set_transform(llgl::Transform {}.translate(Util::Cs::Vector3f::from_deprecated_vector(Util::Vector3f { m_offset / Util::Constants::AU })).matrix(),
        sv.camera().view_matrix(),
        sv.projection().matrix());

    if (static_cast<size_t>(m_length) != m_vertexes.size()) {
        GL::draw_with_temporary_vao<Vertex>(sv.renderer(), shader, uniforms, llgl::PrimitiveType::LineStrip, { m_vertexes.data() + 1, static_cast<size_t>(m_append_offset - 1) });
    }
    else {
        GL::draw_with_temporary_vao<Vertex>(sv.renderer(), shader, uniforms, llgl::PrimitiveType::LineStrip, { m_vertexes.data(), static_cast<size_t>(m_append_offset) });
        GL::draw_with_temporary_vao<Vertex>(sv.renderer(), shader, uniforms, llgl::PrimitiveType::LineStrip, { m_vertexes.data() + m_append_offset, static_cast<size_t>(m_length - m_append_offset) });
    }
}

void Trail::change_current(Util::Vector3d pos) {
    assert(m_length > 0);
    if (m_append_offset == 1)
        m_vertexes[m_length - 1].value<0>() = Util::Vector3f { pos / Util::Constants::AU };
    m_vertexes[m_append_offset - 1].value<0>() = Util::Vector3f { pos / Util::Constants::AU };
}

std::ostream& operator<<(std::ostream& out, Trail const& trail) {
    out << "Trail(" << trail.m_length << " a@ " << trail.m_append_offset << ")";
    return out;
}
