#include "Trail.hpp"
#include "Object.hpp"
#include "Vector2.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <iostream>

Trail::Trail() : m_trail_vertexbuffer(sf::LinesStrip){}

void Trail::push_back(Vector2 pos, Vector2 vel){
    m_trail.push_back({pos, vel});
}

void Trail::push_front(Vector2 pos, Vector2 vel){
    m_trail.push_front({pos, vel});
}

void Trail::pop_front(){
    m_trail.pop_front();
}

void Trail::m_cal_trail(SimulationView const& view, const sf::Color& m_color){
    auto color = m_color;
    color.a = 128;
    if(m_prev_offset != view.offset() || m_prev_zoom != view.scale() || m_prev_window_size != view.window().getSize() || m_refresh){
        m_trail_vertexbuffer.clear();
        
        for(auto& l : m_trail)
            m_trail_vertexbuffer.append(sf::Vertex(view.world_to_screen(l.first), color));
    }else{
        if(m_trail_vertexbuffer.getVertexCount() < m_trail.size())
            m_trail_vertexbuffer.append(sf::Vertex(view.world_to_screen(m_trail.back().first), color));
        else{
            for(unsigned i = 1; i < m_trail.size(); i++)
                m_trail_vertexbuffer[i - 1] = m_trail_vertexbuffer[i];
            m_trail_vertexbuffer[m_trail.size() - 1] = sf::Vertex(sf::Vertex(view.world_to_screen(m_trail.back().first), color));
        }
    }

    m_prev_offset = view.offset();
    m_prev_zoom = view.scale();
    m_prev_window_size = view.window().getSize();

    m_refresh = false;
}

void Trail::draw(SimulationView const& view, sf::Color color){
    auto& target = view.window();
    
    m_cal_trail(view, color);
    target.draw(m_trail_vertexbuffer);
}

bool Trail::reverse_path(bool reverse, Object* obj){    
    if(reverse && m_trail.size() > 1 && m_reverse_con)
    {
        obj->m_pos = m_trail.back().first;
        obj->m_vel = -m_trail.back().second;
        m_trail.pop_back();

        m_refresh = true;
        return 1;
    }

    if(reverse)
        m_reverse_con = 0;

    if(!reverse && m_trail.size() > 1 && !m_reverse_con)
    {
        obj->m_pos = m_trail.back().first;
        obj->m_vel = -m_trail.back().second;
        m_trail.pop_back();
        
        m_refresh = true;
        return 1;
    }

    if(!reverse)
        m_reverse_con = 1;
    
    return 0;
}

