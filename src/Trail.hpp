#pragma once

#include "Vector2.hpp"
#include "gui/SimulationView.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <list>
class Trail{
    std::list<std::pair<Vector2, Vector2>> m_trail;
    sf::VertexArray m_trail_vertexbuffer;
    void m_cal_trail(SimulationView const& view, const sf::Color& m_color);
    Vector2 m_prev_offset;
    double m_prev_zoom;
    bool m_reverse_con = false;
public:
    Trail();
    void push_back(Vector2 pos, Vector2 vel);
    void pop_front();
    void draw(SimulationView const& view, sf::Color color);

    void reverse_path(bool reverse, Vector2& pos, Vector2& vel);
    
    unsigned size() const{return m_trail.size();}
};
