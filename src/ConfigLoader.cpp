#include "ConfigLoader.hpp"

#include "World.hpp"

#include <iostream>
#include <sstream>

void ConfigLoader::load(World& world)
{
    while(true)
    {
        auto planet = parse_planet(world);
        if(!planet)
            break;
        // FIXME: World should store pointers to Objects.
        world.add_object(*planet);
    }
}

std::unique_ptr<Object> ConfigLoader::parse_planet(World& world)
{
    std::string line;
    if(!std::getline(m_in, line))
        return {};

    std::istringstream line_stream(line);

    std::string keyword;
    if(!(line_stream >> keyword)) {
        std::cout << "Expected keyword ('planet')" << std::endl;
        return {};
    }

    std::map<std::string, std::string> properties;

    while(true) {
        auto property = parse_key_value_pair(line_stream);
        if(!property.has_value())
            break;
        properties.insert(property.value());
    }

    auto read_double_property = [&](std::string const& name, double default_value = 0) {
        auto it = properties.find(name);
        if(it == properties.end())
            return default_value;
        return std::stod(it->second);
    };

    // FIXME: It would be better with TRY().
    try
    {
        if(keyword == "planet") {
            // FIXME: This has no overflow checking etc...
            return std::make_unique<Object>(world,
                read_double_property("mass", 1),
                read_double_property("radius", 1000),
                Vector2{ read_double_property("posx"), read_double_property("posy") },
                Vector2{ read_double_property("velx"), read_double_property("vely") },
                sf::Color{ (uint8_t)read_double_property("colorr", 255), (uint8_t)read_double_property("colorg", 255), (uint8_t)read_double_property("colorb", 255) },
                properties["name"],
                read_double_property("trail_length", 1000)
            );
        } else {
            std::cout << "Invalid keyword: " << keyword << ". Must be 'planet'" << std::endl;
        }
    }
    catch(...)
    {
        std::cout << "Some of the values are invalid, but C++ exceptions doesn't say which." << std::endl;
        return {};
    }
    return {};
}

std::optional<std::pair<std::string, std::string>> ConfigLoader::parse_key_value_pair(std::istream& in)
{
    std::string name;
    in >> std::ws;
    while(isalnum(in.peek())) {
        name += in.get();
    }

    in >> std::ws;

    if(in.get() != '=') {
        std::cout << "Expected '='" << std::endl;
        return {};
    }

    in >> std::ws;

    std::string value;
    if(!(in >> value)) {
        std::cout << "Expected value" << std::endl;
        return {};
    }
    std::cout << name << "=" << value << std::endl;
    return {{ name, value }};
}
