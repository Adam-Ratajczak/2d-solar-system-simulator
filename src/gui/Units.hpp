#pragma once

#include <cmath>
#include <ostream>
class Length
{
public:
    enum Unit
    {
        Auto,
        Px,
        PxOtherSide,
        Percent
    };

    constexpr Length() = default;

    constexpr Length(Unit unit)
    : m_unit(unit) {}

    constexpr Length(float v, Unit unit)
    : m_value(v), m_unit(unit) {}

    constexpr Unit unit() const { return m_unit; }
    constexpr float value() const { return m_value; }

    constexpr Length operator-() const { return { -m_value, m_unit }; }

private:
    friend std::ostream& operator<<(std::ostream& out, Length const& l) { return out << l.m_value << "U" << (int)l.unit(); }

    Unit m_unit = Auto;
    float m_value = 0;
};

constexpr Length operator""_px(long double v)
{
    return Length(v, Length::Px);
}

constexpr Length operator""_px_o(long double v)
{
    return Length(v, Length::PxOtherSide);
}

class Angle
{
public:
    enum Unit
    {
        Rad,
        Deg
    };

    constexpr Angle() = default;

    constexpr Angle(Unit unit)
    : m_unit(unit) {}

    constexpr Angle(float v, Unit unit)
    : m_value(v), m_unit(unit) {}

    constexpr float deg() const { return (m_unit == Deg) ? m_value : m_value / 360 * 2 * M_PI; }
    constexpr float rad() const { return (m_unit == Rad) ? m_value : m_value / (2 * M_PI) * 360; }

    constexpr Angle operator-() const { return { -m_value, m_unit }; }

private:
    Unit m_unit = Rad;
    float m_value = 0;
};

constexpr Angle operator""_deg(long double v)
{
    return Angle(v, Angle::Deg);
}

constexpr Angle operator""_rad(long double v)
{
    return Angle(v, Angle::Rad);
}
