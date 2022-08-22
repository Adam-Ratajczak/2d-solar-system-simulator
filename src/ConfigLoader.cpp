#include "ConfigLoader.hpp"

#include "World.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

void ConfigLoader::load(World& world) {
    while (true) {
        if (!parse_statement(world))
            break;
    }
}

bool ConfigLoader::parse_statement(World& world) {
    std::string line;
    if (!std::getline(m_in, line, ';'))
        return {};

    std::istringstream line_stream(line);

    std::string keyword;
    if (!(line_stream >> keyword)) {
        std::cout << "Expected keyword ('planet', 'orbitting_planet' or 'light_source')" << std::endl;
        return {};
    }

    std::map<std::string, std::string> properties;

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
        std::cout << keyword << std::endl;
        if (keyword == "planet") {
            while (true) {
                auto property = parse_key_value_pair(line_stream);
                if (!property.has_value())
                    break;
                properties.insert(property.value());
            }

            // FIXME: This has no overflow checking etc...
            world.add_object(std::make_unique<Object>(
                read_double_property("mass", 1),
                read_distance_property("radius").value(),
                Util::Vector3d { read_double_property("posx"), read_double_property("posy"), read_double_property("posz") },
                Util::Vector3d { read_double_property("velx"), read_double_property("vely"), read_double_property("velz") },
                Util::Color { (uint8_t)read_double_property("colorr", 255), (uint8_t)read_double_property("colorg", 255), (uint8_t)read_double_property("colorb", 255) },
                properties["name"],
                read_double_property("trail_length", 1000 * 3600 * 24)));
        }
        else if (keyword == "orbitting_planet") {
            while (true) {
                auto property = parse_key_value_pair(line_stream);
                if (!property.has_value())
                    break;
                properties.insert(property.value());
            }

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
            if (read_distance_property("apoapsis").value() != 0 && read_distance_property("periapsis").value() != 0) {
                world.add_object(around_object->create_object_relative_to_ap_pe(
                    read_double_property("mass"),
                    read_distance_property("radius"),
                    read_distance_property("apoapsis"),
                    read_distance_property("periapsis"),
                    properties["direction"] == "right" ? false : true,
                    Util::Angle::degrees(read_double_property("orbit_position")),
                    Util::Angle::degrees(read_double_property("orbit_tilt")),
                    Util::Color { (uint8_t)read_double_property("colorr", 255), (uint8_t)read_double_property("colorg", 255), (uint8_t)read_double_property("colorb", 255) },
                    properties["name"],
                    0.0_deg));
            }
            else {
                world.add_object(around_object->create_object_relative_to_maj_ecc(
                    read_double_property("mass"),
                    read_distance_property("radius"),
                    read_distance_property("major_axis"),
                    read_double_property("eccencrity"),
                    properties["direction"] == "right" ? false : true,
                    Util::Angle::degrees(read_double_property("orbit_position")),
                    Util::Angle::degrees(read_double_property("orbit_tilt")),
                    Util::Color { (uint8_t)read_double_property("colorr", 255), (uint8_t)read_double_property("colorg", 255), (uint8_t)read_double_property("colorb", 255) },
                    properties["name"],
                    0.0_deg));
            }
        }
        else if (keyword == "light_source") {
            std::string name;
            if (!(line_stream >> name)) {
                std::cout << "Expected light source name" << std::endl;
                return false;
            }
            if (!world.set_light_source(name)) {
                std::cout << "Light source planet not found" << std::endl;
                return false;
            }
        }
        else {
            std::cout << "Invalid keyword: " << keyword << ". Must be 'planet', 'orbitting_planet' or 'light_source'." << std::endl;
            return false;
        }
    } catch (std::exception& e) {
        std::cout << "Some of the values are invalid: " << e.what() << std::endl;
        return false;
    }
    return true;
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
