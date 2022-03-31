#include "World.hpp"

#include "ConfigLoader.hpp"

#include <fstream>

void prepare_solar(World& world)
{
    // Sun
    // FIXME: Add that nice template like Container::add_widget :^)
    /*world.add_object(
        Object(world, 1.98892e30, 695700 * 1000,
            Vector2(0, 0),
            Vector2(0, 0),
            sf::Color::Yellow, "Sun", 1000));*/
    std::ifstream config_file("../solar.essa");
    ConfigLoader loader(config_file);
    loader.load(world);

    auto& sun = world.get_object("Sun");

    // Other Planets
    sun.add_object_relative_to(3.3e23, 2439, 0.307 * AU, 0.466 * AU, false, 170.5709_deg, sf::Color(80, 78, 81), "Mercury");
    sun.add_object_relative_to(4.8465e24, 6051, 0.718 * AU, 0.728 * AU, false, 263.6570_deg, sf::Color(255, 255, 255), "Venus");
    sun.add_object_relative_to(5.9742e24, 6371, 152100000000, 149075000000, false, 180.0_deg, sf::Color(100, 149, 237), "Earth");
    sun.add_object_relative_to(6.39e23, 3389, 1.382 * AU, 1.666 * AU, false, 290.6297_deg, sf::Color(188, 39, 50), "Mars");
    sun.add_object_relative_to(1.8982e27, 69911, 4.95 * AU, 5.4588 * AU, false, 105.2543_deg, sf::Color(207, 82, 63), "Jupiter");
    sun.add_object_relative_to(5.6836e26, 58232, 9.0412 * AU, 10.1238 * AU, false, 289.4523_deg, sf::Color(207, 186, 178), "Saturn");
    sun.add_object_relative_to(8.6810e25, 25362, 18.2861 * AU, 20.0965 * AU, false, 276.7999_deg, sf::Color(0, 174, 199), "Uranus");
    sun.add_object_relative_to(1.024e26, 24622, 29.81 * AU, 30.33 * AU, false, 282.7192_deg, sf::Color(0, 111, 255), "Neptune");
}
