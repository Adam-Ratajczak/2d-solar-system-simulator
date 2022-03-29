#include "Trail.hpp"
#include "Vector2.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <iostream>

Trail::Trail(unsigned size, Vector2 pos, Vector2 vel) : m_size(size), m_index(0){
    m_arr = new Vector2[m_size * 2];

    for(unsigned i = 0; i < m_size; i++){
        m_arr[i * 2] = pos;
        m_arr[i * 2 + 1] = vel;
    }
}

void Trail::push(Vector2 pos, Vector2 vel){
    m_arr[m_index * 2] = pos;
    m_arr[m_index * 2 + 1] = vel;
    m_index++;

    if(m_index >= m_size)
        m_index = 0;
}

void Trail::pop(){
    if(m_index == m_size - 1){
        m_arr[m_index] = m_arr[0];
        m_index--;
    }else if(m_index == 0){
        m_arr[m_index] = m_arr[m_index + 1];
        m_index = m_size - 1;
    }else{
        m_arr[m_index] = m_arr[m_index + 1];
        m_index--;
    }
}

void Trail::draw(SimulationView const& view, sf::Color color) const{
    sf::VertexArray buffer(sf::LinesStrip, m_size);
    unsigned index = 0;
    color.a = 128;

    for(unsigned i = m_index + 1; i < m_size; i++){
        buffer[index].position = view.world_to_screen(m_arr[i * 2]);
        buffer[index].color = color;
        index++;
    }

    for(unsigned i = 0; i <= m_index; i++){
        buffer[index].position = view.world_to_screen(m_arr[i * 2]);
        buffer[index].color = color;
        index++;
    }

    view.window().draw(buffer);
}

Trail& Trail::operator=(Trail& other){
    delete[] m_arr;

    m_arr = new Vector2[other.m_size * 2];
    m_size = other.m_size;
    m_index = other.m_index;

    for(unsigned i = 0; i < 2 * m_size; i++)
        m_arr[i] = other.m_arr[i];
    
    return *this;
}

Trail::~Trail(){
    // delete[] m_arr;
}
