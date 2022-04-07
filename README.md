# ESSA

https://github.com/Adam-Ratajczak/essa

## Description
**ESSA**, or Extremely Sophisticated Space Application is an ambitious attempt to create an extremely accurate simulator of gravitational forces in an N-body system. Already an extremely recognizable project among the class, and this is just the beginning! Work is already well underway to take the simulation view into the next dimension. You can already create new objects and track the relationships between them. The interface is very intuitive and minimalistic, which is of course an advantage. Also under consideration are ways to increase the precision of the simulation, as well as a preliminary idea for storing a new simulation history system.

The plans for this application are huge, we are going to add an external API in Python, *PySSA* for short, allowing us to manipulate the simulation and extract information through code, and maybe even export the generated solar system graphics. The tool will also have a lot of functionality related to measuring planetary parameters. We also intend to implement time dilation measurement mechanics and a rocket trajectory creator. It will be possible to create a rocket that will fly between two planets. 

Our goal is to create a stable simulation that is consistent with the real world and reasonably attractive. We also want to do simple, however based on basic physics, planetary collisions.

The application is written in modern C++20 and uses the latest additions to this standard, including `concept`, `requires`, `std::optional` and uses the CMake project build system to ensure cross-platform. The work is synchronized using Git, with a repository created on GitHub.

## Project Scope
* Possibly accurate simulation of gravitational forces in an N body system:
    * Numerical counting of planetary positions with a certain accuracy (configurable).
    * Possibility of using Newton's laws of universal gravitation or Einstein's theory of relativity.
    * Finding various parameters of the bodies, e.g. 
* Ability to add new bodies, including with automatic counting of orbits (either exactly or based on various approximations)
* Automatic finding of orbits with specific properties (e.g. passing through a point or orbiting a planet).
* Basic GUI library - widget framework with auto-layout
* *PySSA* - Python API for scripting.

## External dependencies
* SFML - for rendering the interface.
* Python - for scripting
* fmt - for text formatting (not currently a dependency, but this may change due to its usefulness)
* This list is subject to change