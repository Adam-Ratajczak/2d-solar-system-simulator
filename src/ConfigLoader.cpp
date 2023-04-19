#include "ConfigLoader.hpp"

#include "World.hpp"

#include <EssaUtil/GenericParser.hpp>
#include <EssaUtil/Stream/File.hpp>
#include <iostream>
#include <sstream>

Config::ErrorOr<Config::Config> ConfigLoader::load(std::string const& filename, World&) {
    Util::ReadableFileStream stream = TRY(Util::ReadableFileStream::open(filename));
    Util::TextReader reader { stream };
    ConfigLoader loader { reader };
    return loader.parse_config();
}

Config::ErrorOr<Config::Config> ConfigLoader::parse_config() {
    Config::Config config;
    TRY(m_reader.consume_while(isspace));
    while (!m_reader.is_eof()) {
        config.statements.push_back(TRY(parse_statement()));
        TRY(m_reader.consume_while(isspace));
        if (TRY(m_reader.peek()) != ';') {
            return Util::ParseError { "Expected ';'", { m_reader.location(), {} } };
        }
        TRY(m_reader.consume()); // ;
        TRY(m_reader.consume_while(isspace));
    }
    return config;
}

class PropertyMap {
public:
    void insert(std::pair<Util::UString, Util::UString>&& pair) {
        m_properties.insert(std::forward<decltype(pair)>(pair));
    }

    bool contains(Util::UString const& name) {
        return m_properties.contains(name);
    }

    auto get(Util::UString const& name, Util::UString const& default_value = "") -> Util::UString {
        auto it = m_properties.find(name);
        if (it == m_properties.end()) {
            return default_value;
        }
        return it->second;
    }

    auto get_double(Util::UString const& name, double default_value = 0) const -> Config::ErrorOr<double> {
        auto it = m_properties.find(name);
        if (it == m_properties.end())
            return default_value;
        try {
            return it->second.parse<double>();
        } catch (...) {
            return Util::ParseError { "Invalid number", {} };
        }
    };

    auto get_distance(Util::UString const& name) const -> Config::ErrorOr<Distance> {
        auto it = m_properties.find(name);
        if (it == m_properties.end())
            return Distance {};
        auto underscore_index = it->second.find("_");
        if (!underscore_index)
            return Distance { static_cast<float>(TRY(get_double(name))), Distance::Unit::Meter };
        auto unit = it->second.substring(underscore_index.value() + 1);
        // std::cout << name << ": " << unit << std::endl;
        auto value = TRY(it->second.substring(0, underscore_index.value()).parse<float>());
        if (unit == "m")
            return Distance(value, Distance::Unit::Meter);
        if (unit == "km")
            return Distance(value * 1000, Distance::Unit::Kilometer);
        if (unit == "AU")
            return Distance(value * Util::Constants::AU, Distance::Unit::Au);
        return Util::ParseError { "Invalid unit", {} };
    };

    auto get_int(Util::UString const& name, int default_value) const -> Config::ErrorOr<int> {
        auto it = m_properties.find(name);
        if (it == m_properties.end())
            return default_value;
        try {
            return it->second.parse<int>();
        } catch (...) {
            return Util::ParseError { name.encode() + " is an invalid number", {} };
        }
    };

    auto get_byte(Util::UString const& name, uint8_t default_value) const -> Config::ErrorOr<uint8_t> {
        auto it = m_properties.find(name);
        if (it == m_properties.end())
            return default_value;
        try {
            auto value = std::stoi(it->second.encode());
            if (value < 0 || value > 255) {
                return Util::ParseError { name.encode() + " must be in range 0..256", {} };
            }
            return static_cast<uint8_t>(value);
        } catch (...) {
            return Util::ParseError { name.encode() + " is an invalid number", {} };
        }
    };

private:
    std::map<Util::UString, Util::UString> m_properties;
};

Config::ErrorOr<Config::Statement> ConfigLoader::parse_statement() {
    auto keyword = TRY(m_reader.consume_while([](uint8_t c) { return isalpha(c) || c == '_'; }));
    if (keyword.is_empty()) {
        return Util::ParseError { "Expected keyword", { m_reader.location(), {} } };
    }

    TRY(m_reader.consume_while(isspace));

    auto read_properties = [this]() -> Config::ErrorOr<PropertyMap> {
        PropertyMap properties;
        while (true) {
            properties.insert(TRY(parse_key_value_pair()));
            TRY(m_reader.consume_while(isspace));
            auto peek = TRY(m_reader.peek());
            if (!peek) {
                return Util::ParseError { "Expected ';' after property list", { m_reader.location(), {} } };
            }
            if (!isalpha(*peek)) {
                return properties;
            }
        }
    };

    auto read_planet_common_properties = [&](PropertyMap& properties, Config::Planet& planet) -> Config::ErrorOr<void> {
        planet.mass = TRY(properties.get_double("mass"));
        planet.radius = TRY(properties.get_distance("radius"));
        planet.color = { TRY(properties.get_byte("colorr", 255)), TRY(properties.get_byte("colorg", 255)), TRY(properties.get_byte("colorb", 255)) };
        planet.name = Util::UString { properties.get("name") };
        planet.trail_length = TRY(properties.get_int("trail_length", 600));
        return {};
    };

    if (keyword == "planet") {
        PropertyMap properties = TRY(read_properties());
        Config::AbsolutePlanet planet;
        TRY(read_planet_common_properties(properties, planet));
        planet.position = { TRY(properties.get_double("posx")), TRY(properties.get_double("posy")), TRY(properties.get_double("posz")) };
        planet.velocity = { TRY(properties.get_double("velx")), TRY(properties.get_double("vely")), TRY(properties.get_double("velz")) };
        return planet;
    }
    if (keyword == "orbiting_planet") {
        PropertyMap properties = TRY(read_properties());
        Config::AbsolutePlanet planet;

        auto read_orbiting_planet_common_properties = [&](Config::OrbitingPlanet& planet) -> Config::ErrorOr<void> {
            TRY(read_planet_common_properties(properties, planet));
            planet.orbit_position = Util::Angle::degrees(TRY(properties.get_double("orbit_position")));
            planet.orbit_tilt = Util::Angle::degrees(TRY(properties.get_double("orbit_tilt")));
            planet.around = Util::UString { properties.get("around") };
            planet.direction = properties.get("direction") == "left" ? Config::Direction::CounterClockwise : Config::Direction::Clockwise;
            return {};
        };

        if (properties.contains("apoapsis") && properties.contains("periapsis")) {
            Config::ApPeDefinedOrbitingPlanet planet;
            TRY(read_orbiting_planet_common_properties(planet));
            planet.apoapsis = TRY(properties.get_distance("apoapsis"));
            planet.periapsis = TRY(properties.get_distance("periapsis"));
            return planet;
        }
        if (properties.contains("major_axis") && properties.contains("eccentrity")) {
            Config::EccentrityDefinedOrbitingPlanet planet;
            TRY(read_orbiting_planet_common_properties(planet));
            planet.major_axis = TRY(properties.get_distance("major_axis"));
            planet.eccentrity = TRY(properties.get_double("eccentrity"));
            return planet;
        }
        return Util::ParseError { "orbiting_planet must define apoapsis+periapsis or major_axis+eccentrity", { m_reader.location(), {} } };
    }
    if (keyword == "light_source") {
        auto name = TRY(m_reader.consume_while(isalnum));
        return Config::LightSource { Util::UString { name } };
    }

    return Util::ParseError { "Invalid statement keyword: '" + keyword.encode() + "'", { m_reader.location(), {} } };
}

Config::ErrorOr<std::pair<Util::UString, Util::UString>> ConfigLoader::parse_key_value_pair() {
    TRY(m_reader.consume_while(isspace));
    Util::UString name = TRY(m_reader.consume_while([](uint8_t c) { return isalpha(c) || c == '_'; }));

    TRY(m_reader.consume_while(isspace));
    auto equal = TRY(m_reader.consume());
    if (equal != '=') {
        return Util::ParseError { "Expected '='", { m_reader.location(), {} } };
    }

    TRY(m_reader.consume_while(isspace));
    auto value = TRY(m_reader.consume_while([](uint8_t c) { return c != ';' && !isspace(c); }));

    // fmt::print("'{}'='{}'\n", name, value);
    return std::pair { std::move(name), std::move(value) };
}

Config::ErrorOr<void> Config::Config::apply(World& world) {
    // TODO: Use UString in Object
    for (auto const& stmt : statements) {
        TRY(std::visit(
            Util::Overloaded {
                [&world](AbsolutePlanet const& planet) -> ErrorOr<void> {
                    fmt::print("{}\n", planet.name.encode());
                    world.add_object(std::make_unique<Object>(
                        planet.mass,
                        planet.radius.value(),
                        planet.position,
                        planet.velocity,
                        planet.color,
                        planet.name,
                        planet.trail_length));
                    return {};
                },
                [&world](ApPeDefinedOrbitingPlanet const& planet) -> ErrorOr<void> {
                    auto* around = world.get_object_by_name(planet.around);
                    if (!around) {
                        return Util::ParseError { "Invalid planet for orbiting around: '" + planet.around.encode() + "'" };
                    }
                    fmt::print("{}\n", planet.name.encode());
                    world.add_object(around->create_object_relative_to_ap_pe(
                        planet.mass,
                        planet.radius,
                        planet.apoapsis,
                        planet.periapsis,
                        planet.direction == Direction::Clockwise,
                        planet.orbit_position,
                        planet.orbit_tilt,
                        planet.color,
                        planet.name,
                        0.0_deg));
                    return {};
                },
                [&world](EccentrityDefinedOrbitingPlanet const& planet) -> ErrorOr<void> {
                    auto* around = world.get_object_by_name(planet.around);
                    if (!around) {
                        return Util::ParseError { "Invalid planet for orbiting around: '" + planet.around.encode() + "'" };
                    }
                    world.add_object(around->create_object_relative_to_maj_ecc(
                        planet.mass,
                        planet.radius,
                        planet.major_axis,
                        planet.eccentrity,
                        planet.direction == Direction::Clockwise,
                        planet.orbit_position,
                        planet.orbit_tilt,
                        planet.color,
                        planet.name,
                        0.0_deg));
                    return {};
                },
                [&world](LightSource const& source) -> ErrorOr<void> {
                    auto* planet = world.get_object_by_name(source.planet_name);
                    if (!planet) {
                        return Util::ParseError { "Invalid planet for light source: '" + source.planet_name.encode() + "'" };
                    }
                    world.set_light_source(planet);
                    return {};
                },
            },
            stmt));
    }
    return {};
}
