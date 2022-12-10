#pragma once

#include "Object.hpp"

#include <EssaUtil/GenericParser.hpp>
#include <EssaUtil/Stream.hpp>
#include <istream>
#include <memory>
#include <optional>

namespace Config {

struct Planet {
    double mass;
    Distance radius;
    Util::Color color;
    Util::UString name;
    double trail_length;
};

struct AbsolutePlanet : public Planet {
    Util::Vector3d position;
    Util::Vector3d velocity;
};

enum class Direction {
    Clockwise,
    CounterClockwise
};

struct OrbitingPlanet : public Planet {
    Util::UString around;
    Direction direction;
    Util::Angle orbit_position;
    Util::Angle orbit_tilt;
};

struct ApPeDefinedOrbitingPlanet : public OrbitingPlanet {
    Distance apoapsis;
    Distance periapsis;
};

struct EccentrityDefinedOrbitingPlanet : public OrbitingPlanet {
    Distance major_axis;
    double eccentrity;
};

struct LightSource {
    Util::UString planet_name;
};

using Statement = std::variant<AbsolutePlanet, ApPeDefinedOrbitingPlanet, EccentrityDefinedOrbitingPlanet, LightSource>;

template<class T>
using ErrorOr = Util::ErrorOr<T, Util::ParseError, Util::OsError>;

struct Config {
    std::vector<Statement> statements;

    ErrorOr<void> apply(World&);
};

}

class ConfigLoader {
public:
    static Config::ErrorOr<Config::Config> load(std::string const& filename, World& world);

private:
    explicit ConfigLoader(Util::TextReader& reader)
        : m_reader(reader) { }

    Config::ErrorOr<Config::Config> parse_config();
    Config::ErrorOr<Config::Statement> parse_statement();
    Config::ErrorOr<std::pair<Util::UString, Util::UString>> parse_key_value_pair();

    Util::TextReader& m_reader;
};
