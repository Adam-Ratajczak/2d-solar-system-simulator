#include "World.hpp"

#include "ConfigLoader.hpp"

#include <fstream>

void prepare_solar(World& world)
{
    // Sun
    std::ifstream config_file("../solar.essa");
    ConfigLoader loader(config_file);
    loader.load(world);

    // Other Planets
    auto sun = world.get_object_by_name("Sun");
    if(!sun)
        return;
    sun->add_object_relative_to(3.3e23, 2439.0_km, 0.307_AU, 0.466_AU, true, 170.5709_deg, 20.0_deg, sf::Color(80, 78, 81), "Mercury");
    sun->add_object_relative_to(4.8465e24, 6051.0_km, 0.718_AU, 0.728_AU, true, 263.6570_deg, -10.0_deg, sf::Color(255, 255, 255), "Venus");
    sun->add_object_relative_to(5.9742e24, 6371.0_km, 152100000.0_km, 149075000.0_km, true, 180.0_deg, 15.0_deg, sf::Color(100, 149, 237), "Earth");
    sun->add_object_relative_to(6.39e23, 3389.0_km, 1.382_AU, 1.666_AU, true, 290.6297_deg, -5.0_deg, sf::Color(188, 39, 50), "Mars");
    sun->add_object_relative_to(1.8982e27, 69911.0_km, 4.95_AU, 5.4588_AU, true, 105.2543_deg, 0.0_deg, sf::Color(207, 82, 63), "Jupiter");
    sun->add_object_relative_to(5.6836e26, 58232.0_km, 9.0412_AU, 10.1238_AU, true, 289.4523_deg, 0.0_deg, sf::Color(207, 186, 178), "Saturn");
    sun->add_object_relative_to(8.6810e25, 25362.0_km, 18.2861_AU, 20.0965_AU, true, 276.7999_deg, 0.0_deg, sf::Color(0, 174, 199), "Uranus");
    sun->add_object_relative_to(1.024e26, 24622.0_km, 29.81_AU, 30.33_AU, true, 282.7192_deg, 0.0_deg, sf::Color(0, 111, 255), "Neptune");

    auto earth = world.get_object_by_name("Earth");
    earth->add_object_relative_to(7.342e22, 1737.0_km, 405400.0_km, 362400.0_km, true, 108.0_deg, 0.0_deg, sf::Color(127, 127, 127), "Moon");
}
