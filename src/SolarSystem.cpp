#include "World.hpp"

void prepare_solar(World& world){
    // Sun
    world.add_object(
        Object(1.98892e30, 695700 * 1000, 
        Vector2(0, 0), 
        Vector2(0, 0), 
        sf::Color::Yellow, "Sun", 1000));

    // Mercury
    world.add_object(
        Object(3.3e23, 2439 * 1000, 
        Vector2(-0.387 * AU, 0), 
        Vector2(0, -47400), 
        sf::Color(80, 78, 81), "Mercury", 100));

    // Venus
    world.add_object(
        Object(4.8465e24, 6051 * 1000, 
        Vector2(-0.723 * AU, 0), 
        Vector2(0, -35020), 
        sf::Color(255, 255, 255), "Venus", 300));

    // Earth
    world.add_object(
        Object(5.9742e24, 6371 * 1000, 
        Vector2(-1 * AU, 0), 
        Vector2(0, -29783), 
        sf::Color(100, 149, 237), "Earth", 600));
    
    // Mars
    world.add_object(
        Object(6.39e23, 3389 * 1000, 
        Vector2(-1.524 * AU, 0), 
        Vector2(0, -24077), 
        sf::Color(188, 39, 50), "Mars", 1000));

    // Jupiter
    world.add_object(
        Object(1.8982e27, 69911 * 1000, 
        Vector2(-5.203 * AU, 0), 
        Vector2(0, -13070), 
        sf::Color(207, 82, 63), "Jupiter", 5000));

    // Saturn
    world.add_object(
        Object(5.6836e26, 58232 * 1000, 
        Vector2(-9.539 * AU, 0), 
        Vector2(0, -9680), 
        sf::Color(207, 186, 178), "Saturn", 12500));

    // Uranus
    world.add_object(
        Object(8.6810e25, 25362 * 1000, 
        Vector2(-19.18 * AU, 0), 
        Vector2(0, -6800), 
        sf::Color(0, 174, 199), "Uranus", 35000));

    // Neptune
    world.add_object(
        Object(1.024e26, 24622 * 1000, 
        Vector2(-30.06 * AU, 0), 
        Vector2(0, -5430), 
        sf::Color(0, 111, 255), "Neptune", 70000));
}