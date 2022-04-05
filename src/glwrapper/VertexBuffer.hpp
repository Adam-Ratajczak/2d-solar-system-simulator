#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>
#include "../Vector3.hpp"
#include "Color.hpp"

class VertexBuffer{
public:
    VertexBuffer() = default;
    VertexBuffer(unsigned vertices);

    void draw(sf::RenderWindow& window) const;
    void append_vertex(Vector3 vertex, Color color);
    Vertex operator[](unsigned index) const;
    Vertex& operator[](unsigned index);
    unsigned size() const;
    void resize(unsigned to_resize);
    void clear();

private:
    std::vector<Vertex> m_vertices;
};
