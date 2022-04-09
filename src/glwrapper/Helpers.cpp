#include "Helpers.hpp"

namespace GL {

void draw_vertices(GLenum mode, std::span<Vertex const> vertices) {
    glVertexPointer(3, GL_DOUBLE, sizeof(Vertex), (char*)vertices.data() + offsetof(Vertex, position));
    glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(Vertex), (char*)vertices.data() + offsetof(Vertex, color));
    glDrawArrays(mode, 0, vertices.size());
}

void draw_indexed_vertices(GLenum mode, std::span<Vertex const> vertices, std::span<unsigned const> indices) {
    glVertexPointer(3, GL_DOUBLE, sizeof(Vertex), (char*)vertices.data() + offsetof(Vertex, position));
    glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(Vertex), (char*)vertices.data() + offsetof(Vertex, color));
    glDrawElements(mode, indices.size(), GL_UNSIGNED_INT, indices.data());

}

}
