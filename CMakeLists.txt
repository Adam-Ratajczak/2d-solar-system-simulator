cmake_minimum_required(VERSION 3.19)
project(out)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

find_package(SFML 2.5.1 COMPONENTS graphics audio REQUIRED)

add_executable(
    out
    src/Object.cpp
    src/SolarSystem.cpp
    src/World.cpp
    src/main.cpp
    src/gui/Button.cpp
    src/gui/Container.cpp
    src/gui/GUI.cpp
    src/gui/SimulationView.cpp
    src/gui/Slider.cpp
    src/gui/Textbox.cpp
    src/gui/Textfield.cpp
    src/gui/ToggleButton.cpp
    src/gui/Widget.cpp
)
target_link_libraries(
    out
    sfml-graphics
    sfml-window
)
