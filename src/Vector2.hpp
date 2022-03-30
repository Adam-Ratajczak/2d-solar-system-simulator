#pragma once

#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <ostream>
class Vector2{
public:
    double x, y;

    Vector2() : x(0.), y(0.){}
    Vector2(double _x, double _y) : x(_x), y(_y){}
    Vector2(sf::Vector2f _a) : x(_a.x), y(_a.y){}
    Vector2(sf::Vector2u _a) : x(_a.x), y(_a.y){}
    Vector2(sf::Vector2i _a) : x(_a.x), y(_a.y){}

    double magnitude() const;
    double angle() const;

    void limit(double mag);
    void set_magnitude(double mag);

    Vector2 rotate_vector(double theta);

    Vector2& operator=(sf::Vector2f b){
        this->x = b.x; 
        this->y = b.y; 
        return *this;
    }

    operator sf::Vector2f(){
        return sf::Vector2f(x, y);
    }

    operator sf::Vector2u(){
        return sf::Vector2u(x, y);
    }

    operator sf::Vector2i(){
        return sf::Vector2i(x, y);
    }

    double distance_to(Vector2 const& other) const
    {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx*dx+dy*dy);
    }
};

inline Vector2 operator+(Vector2 a, Vector2 b){return Vector2(a.x + b.x, a.y + b.y);}
inline Vector2 operator-(Vector2 a, Vector2 b){return Vector2(a.x - b.x, a.y - b.y);}
inline Vector2 operator*(Vector2 a, double b){return Vector2(a.x * b, a.y * b);}
inline Vector2 operator*(double a, Vector2 b){return Vector2(a * b.x, a * b.y);}
inline Vector2 operator/(Vector2 a, double b){return Vector2(a.x / b, a.y / b);}
inline Vector2 operator-(Vector2 a){return Vector2(-a.x, -a.y);}

inline Vector2& operator+=(Vector2& a, Vector2 b){return a = a + b;}
inline Vector2& operator-=(Vector2& a, Vector2 b){return a = a - b;}
inline Vector2& operator*=(Vector2& a, double b){return a = a * b;}
inline Vector2& operator/=(Vector2& a, double b){return a = a / b;}

inline bool operator==(Vector2& a, Vector2& b){return a.x == b.x && a.y == b.y;}
inline bool operator!=(Vector2& a, Vector2& b){return !(a == b);}
inline bool operator==(const Vector2& a, const Vector2& b){return a.x == b.x && a.y == b.y;}
inline bool operator!=(const Vector2& a, const Vector2& b){return !(a == b);}

inline std::ostream& operator<<(std::ostream& out, Vector2 vec){return out << "(" << vec.x << ", " << vec.y << ")";}

inline double Vector2::magnitude() const{
    return std::sqrt(x * x + y * y);
}

inline double Vector2::angle() const{
    return(std::atan2(y, x));
}

inline Vector2 Vector2::rotate_vector(double theta){
    double t_cos = std::cos(theta), t_sin = std::sin(theta);

    return Vector2(x * t_cos - y * t_sin, x * t_sin + y * t_cos);
}

inline void Vector2::limit(double mag){
    double d = this->magnitude();

    if(d < mag)
        return;
    
    double theta = this->angle();

    x = std::cos(theta) * mag;
    y = std::sin(theta) * mag;
}

inline void Vector2::set_magnitude(double mag){
    double theta = this->angle();

    x = std::cos(theta) * mag;
    y = std::sin(theta) * mag;
}

inline double get_distance(Vector2 a, Vector2 b){
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}
