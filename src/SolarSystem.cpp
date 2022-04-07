#include "World.hpp"

#include "ConfigLoader.hpp"

#include <fstream>

void prepare_solar(World& world)
{
    // Sun
    // FIXME: Add that nice template like Container::add_widget :^)
    world.add_object(
        std::make_unique<Object>(world, 1.98892e30, 695700 * 1000,
            Vector3(0, 0),
            Vector3(0, 0),
            sf::Color::Yellow, "Sun", 1000));
    // std::ifstream config_file("../solar.essa");
    // ConfigLoader loader(config_file);
    // loader.load(world);

    // Other Planets
    auto sun = world.get_object_by_name("Sun");
    if(!sun)
        return;
    sun->add_object_relative_to(3.3e23, 2439.0_km, 0.307_AU, 0.466_AU, false, 170.5709_deg, 0.0_deg, sf::Color(80, 78, 81), "Mercury");
    sun->add_object_relative_to(4.8465e24, 6051.0_km, 0.718_AU, 0.728_AU, false, 263.6570_deg, 0.0_deg, sf::Color(255, 255, 255), "Venus");
    sun->add_object_relative_to(5.9742e24, 6371.0_km, 152100000.0_km, 149075000.0_km, false, 180.0_deg, 0.0_deg, sf::Color(100, 149, 237), "Earth");
    sun->add_object_relative_to(6.39e23, 3389.0_km, 1.382_AU, 1.666_AU, false, 290.6297_deg, 0.0_deg, sf::Color(188, 39, 50), "Mars");
    sun->add_object_relative_to(1.8982e27, 69911.0_km, 4.95_AU, 5.4588_AU, false, 105.2543_deg, 0.0_deg, sf::Color(207, 82, 63), "Jupiter");
    sun->add_object_relative_to(5.6836e26, 58232.0_km, 9.0412_AU, 10.1238_AU, false, 289.4523_deg, 0.0_deg, sf::Color(207, 186, 178), "Saturn");
    sun->add_object_relative_to(8.6810e25, 25362.0_km, 18.2861_AU, 20.0965_AU, false, 276.7999_deg, 0.0_deg, sf::Color(0, 174, 199), "Uranus");
    sun->add_object_relative_to(1.024e26, 24622.0_km, 29.81_AU, 30.33_AU, false, 282.7192_deg, 0.0_deg, sf::Color(0, 111, 255), "Neptune");
}
