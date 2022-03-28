#include "World.hpp"

void prepare_solar(World& world){
    // Sun
    world.add_object(
        Object(1.98892e30, 695700 * 1000, 
        Vector2(0, 0), 
        Vector2(0, 0), 
        sf::Color::Yellow, "Sun", 1000));

    auto& sun = world.get_object("Sun");

    // Other Planets
    sun.add_object(3.3e23, 2439 * 1000, 0.387 * AU, 170.5709_deg, -47400, sf::Color(80, 78, 81), "Mercury", 100);
    sun.add_object(4.8465e24, 6051 * 1000, 0.723 * AU, 263.6570_deg, -35020, sf::Color(255, 255, 255), "Venus", 300);
    sun.add_object(5.9742e24, 6371 * 1000, 1 * AU, 180.0_deg, -29783, sf::Color(100, 149, 237), "Earth", 600);
    sun.add_object(6.39e23, 3389 * 1000, 1.524 * AU, 290.6297_deg, -24077, sf::Color(188, 39, 50), "Mars", 1000);
    sun.add_object(1.8982e27, 69911 * 1000, 5.203 * AU, 105.2543_deg, -13070, sf::Color(207, 82, 63), "Jupiter", 5000);
    sun.add_object(5.6836e26, 58232 * 1000, 9.539 * AU, 289.4523_deg, -9680, sf::Color(207, 186, 178), "Saturn", 12500);
    sun.add_object(8.6810e25, 25362 * 1000, 19.18 * AU, 276.7999_deg, -6800, sf::Color(0, 174, 199), "Uranus", 35000);
    sun.add_object(1.024e26, 24622 * 1000, 30.06 * AU, 282.7192_deg, -5430, sf::Color(0, 111, 255), "Neptune", 70000);
    
}