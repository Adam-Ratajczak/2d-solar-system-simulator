#include "ConfigLoader.hpp"

#include "World.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

void ConfigLoader::load(World& world) {
    while (true) {
        auto planet = parse_planet(world);
        if (!planet)
            break;
        // FIXME: World should store pointers to Objects.
        world.add_object(std::move(planet));
    }
}

std::unique_ptr<Object> ConfigLoader::parse_planet(World& world) {
    std::string line;
    if (!std::getline(m_in, line, ';'))
        return {};

    std::istringstream line_stream(line);

    std::string keyword;
    if (!(line_stream >> keyword)) {
        std::cout << "Expected keyword ('planet' or 'orbitting_planet')" << std::endl;
        return {};
    }

    std::map<std::string, std::string> properties;

    while (true) {
        auto property = parse_key_value_pair(line_stream);
        if (!property.has_value())
            break;
        properties.insert(property.value());
    }

    auto read_double_property = [&](std::string const& name, double default_value = 0) {
        auto it = properties.find(name);
        if (it == properties.end())
            return default_value;
        return std::stod(it->second);
    };

    auto read_distance_property = [&](std::string const& name) -> Distance {
        auto it = properties.find(name);
        if (it == properties.end())
            return {};
        auto underscore_index = it->second.find("_");
        if (underscore_index == std::string::npos)
            return Distance { static_cast<float>(read_double_property(name)), Distance::Unit::Meter };
        auto unit = it->second.substr(underscore_index + 1);
        // std::cout << name << ": " << unit << std::endl;
        if (unit == "m")
            return Distance(std::stod(it->second.substr(0, underscore_index)), Distance::Unit::Meter);
        if (unit == "km")
            return Distance(std::stod(it->second.substr(0, underscore_index)) * 1000, Distance::Unit::Kilometer);
        if (unit == "AU")
            return Distance(std::stod(it->second.substr(0, underscore_index)) * AU, Distance::Unit::Au);
        throw std::runtime_error("invalid unit");
    };

    // FIXME: It would be better with TRY().
    try {
        if (keyword == "planet") {
            // FIXME: This has no overflow checking etc...
            return std::make_unique<Object>(
                read_double_property("mass", 1),
                read_double_property("radius", 1000),
                Vector3 { read_double_property("posx"), read_double_property("posy"), read_double_property("posz") },
                Vector3 { read_double_property("velx"), read_double_property("vely"), read_double_property("velz") },
                sf::Color { (uint8_t)read_double_property("colorr", 255), (uint8_t)read_double_property("colorg", 255), (uint8_t)read_double_property("colorb", 255) },
                properties["name"],
                read_double_property("trail_length", 1000));
        }
        else if (keyword == "orbitting_planet") {
            auto around = properties["around"];
            if (around.empty()) {
                std::cout << "You need to specify 'around' for 'orbitting_planet'" << std::endl;
                return {};
            }
            auto around_object = world.get_object_by_name(around);
            if (!around_object) {
                std::cout << "'around' object doesn't exist: " << around << std::endl;
                return {};
            }
            return around_object->create_object_relative_to(
                read_double_property("mass"),
                read_distance_property("radius"),
                read_distance_property("apoapsis"),
                read_distance_property("periapsis"),
                properties["direction"] == "right" ? false : true,
                Angle(read_double_property("orbit_position"), Angle::Unit::Deg),
                Angle(read_double_property("orbit_tilt"), Angle::Unit::Deg),
                sf::Color { (uint8_t)read_double_property("colorr", 255), (uint8_t)read_double_property("colorg", 255), (uint8_t)read_double_property("colorb", 255) },
                properties["name"],
                0.0_deg); // TODO
        }
        else
            std::cout << "Invalid keyword: " << keyword << ". Must be 'planet' or 'orbitting_planet'." << std::endl;
    }

    catch (std::exception& e) {
        std::cout << "Some of the values are invalid: " << e.what() << std::endl;
        return {};
    }
    return {};
}

std::optional<std::pair<std::string, std::string>> ConfigLoader::parse_key_value_pair(std::istream& in) {
    std::string name;
    in >> std::ws;
    while (isalnum(in.peek()) || in.peek() == '_') {
        name += in.get();
    }

    in >> std::ws;

    if (in.get() != '=') {
        std::cout << "Expected '='" << std::endl;
        return {};
    }

    in >> std::ws;

    std::string value;
    if (!(in >> value)) {
        std::cout << "Expected value" << std::endl;
        return {};
    }
    // std::cout << name << "=" << value << std::endl;
    return { { name, value } };
}
