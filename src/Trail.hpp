#pragma once

#include "Vector2.hpp"
#include "gui/SimulationView.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
class Trail{
    Vector2* m_arr = nullptr;
    unsigned m_size;
    unsigned m_index;
public:
    Trail(unsigned size, Vector2 pos, Vector2 vel);
    void push(Vector2 pos, Vector2 vel);
    void pop();
    void draw(SimulationView const& view, sf::Color color) const;

    Vector2 pos(unsigned index) const { return m_arr[index * 2]; }
    Vector2 val(unsigned index) const { return m_arr[index * 2 + 1]; }
    std::pair<Vector2, Vector2> back() const {return {m_arr[m_index * 2], m_arr[m_index * 2 + 1]};}
    unsigned size() const{return m_size;}
    Trail& operator=(Trail& other);

    ~Trail();
};
