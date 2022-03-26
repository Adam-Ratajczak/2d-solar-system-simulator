cmake_minimum_required(VERSION 3.19)
project(out)

set(CMAKE_CXX_STANDARD_REQUIRED true)
# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

find_package(SFML 2.5.1 COMPONENTS graphics audio REQUIRED)

add_executable(
    out
    src/main.cpp
    src/Planet.cpp
    src/World.cpp
    src/Object.cpp
    src/Moon.cpp
    src/gui/Slider.cpp
    src/SolarSystem.cpp
)
target_link_libraries(
    out
    sfml-graphics
    sfml-window
)
