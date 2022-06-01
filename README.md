# ESSA

https://github.com/Adam-Ratajczak/essa

## Description
**ESSA**, or Extremely Sophisticated Space Application is an ambitious attempt to create an extremely accurate simulator of gravitational forces in an N-body system. You can create new objects and track the relationships between them. It is possible to create your own planetar systems or use pre-defined templates, like Solar System or Jupiter System.

Our main goal is to create a stable simulation that is consistent with the real world and reasonably attractive.

The application is written in modern C++20 and uses the latest additions to this standard, including concepts, modern STL classes (like `std::optional`) or constant evaluation `consteval`. It uses the CMake build system to ensure cross-platform [1].

* [1] Currently ESSA is tested only on Linux. Not fully-functional Windows builds are provided.

## Features
* Simulation of gravitational forces in an N-body system:
    * Numerical counting of planetary positions, with configurable tick length
    * The [Leapfrog KDK integrator](http://courses.physics.ucsd.edu/2019/Winter/physics141/Lectures/Lecture2/volker.pdf) is used for as accurate as possible physics while keeping good performance - Solar System template runs at ~67500 iterations per second on AMD Ryzen 5 5600H CPU, with default settings (10 iterations/tick)
* Ability to add new bodies, including with automatic counting of orbits (either exactly or based on various approximations)
* [Basic GUI library](https://github.com/sppmacd/EssaGUI) - widget/GUI framework with auto-layout and simple windowing system, inspired by Qt.
* Simple Python API for scripting, [*PySSA*](./src/pyssa).

## How to build and run this?

If you are on Windows, you can download the prebuilt binaries in Releases page. Note that it doesn't support PySSA; you would need to build from source to use Python scripting.

If you are on Linux, you can use [build instructions](./docs/BuildInstructions.md) to build ESSA from source.

# Documentation

* [PySSA documentation](./docs/PySSA/Index.md)
