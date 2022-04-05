#pragma once

#include <SFML/Graphics/Color.hpp>
#include <cstdint>
#include "../Vector3.hpp"

class Color{
    unsigned m_colors;
public:
    uint8_t r, g, b, a;
    Color()
    : r(0)
    , g(0)
    , b(0)
    , a(255)
    , m_colors(0){}

    Color(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 255)
    : r(_r)
    , g(_g)
    , b(_b)
    , a(_a)
    , m_colors(1){}

    Color(const sf::Color color)
    : r(color.r)
    , g(color.g)
    , b(color.b)
    , a(color.a)
    , m_colors(1){}

    operator sf::Color(){
        return sf::Color(r, g, b, a);
    }

    Color operator+(const Color& color) const{
        Color result(*this);
        result.m_colors += color.m_colors;
        result.r = (this->m_colors * result.r + color.m_colors * color.r) / result.m_colors;
        result.g = (this->m_colors * result.g + color.m_colors * color.g) / result.m_colors;
        result.b = (this->m_colors * result.b + color.m_colors * color.b) / result.m_colors;
        result.a = (this->m_colors * result.a + color.m_colors * color.a) / result.m_colors;

        return result;
    }

    Color operator*(const double mul) const{
        Color result(*this);
        result.r *= mul;
        result.g *= mul;
        result.b *= mul;
        result.a *= mul;

        return result;
    }

    Color& operator+=(const Color& color){
        *this = *this + color;

        return *this;
    }

    Color& operator*=(const double mul){
        *this = *this * mul;

        return *this;
    }

    void glDraw() const{
        glColor4f((float)r / 255, (float)g / 255, (float)b / 255, (float)a / 255);
    }
};

struct Vertex{
    Vector3 position;
    Color color;
};
