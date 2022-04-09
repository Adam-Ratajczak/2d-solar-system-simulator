#pragma once

#include "Vertex.hpp"

#include <span>

namespace GL
{

void draw_vertices(GLenum mode, std::span<Vertex const>);
void draw_indexed_vertices(GLenum mode, std::span<Vertex const>, std::span<unsigned const> indices);

}
