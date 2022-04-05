#include "VertexBuffer.hpp"

VertexBuffer::VertexBuffer(unsigned vertices){
    m_vertices.resize(vertices);
}

void VertexBuffer::draw(sf::RenderWindow& window) const{
    window.popGLStates();

    glBegin(GL_LINES);
        for(unsigned i = 0; i < m_vertices.size() - 1; i++){
            m_vertices[i].color.glDraw();
            m_vertices[i].position.glDraw();
            m_vertices[i + 1].color.glDraw();
            m_vertices[i + 1].position.glDraw();
        }
    glEnd();

    window.pushGLStates();
}

void VertexBuffer::append_vertex(Vector3 vertex, Color color){
    m_vertices.push_back({vertex, color});
}

Vertex VertexBuffer::operator[](unsigned index) const{
    return m_vertices[index];
}

Vertex& VertexBuffer::operator[](unsigned index){
    return m_vertices[index];
}

unsigned VertexBuffer::size() const{
    return m_vertices.size();
}

void VertexBuffer::resize(unsigned int to_resize){
    m_vertices.resize(to_resize);
}

void VertexBuffer::clear(){
    m_vertices.clear();
}
