cmake_minimum_required(VERSION 3.19)
project(out)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(SFML 2.5.1 COMPONENTS graphics audio REQUIRED)

add_executable(
    out
    src/ConfigLoader.cpp
    src/Object.cpp
    src/SolarSystem.cpp
    src/Trail.cpp
    src/World.cpp
    src/main.cpp
    src/gui/Button.cpp
    src/gui/ColorPicker.cpp
    src/gui/Container.cpp
    src/gui/CreateObjectGUI.cpp
    src/gui/GUI.cpp
    src/gui/Application.cpp
    src/gui/SimulationView.cpp
    src/gui/Slider.cpp
    src/gui/Textbox.cpp
    src/gui/Textfield.cpp
    src/gui/ToggleButton.cpp
    src/gui/ValueSlider.cpp
    src/gui/Widget.cpp
)
target_link_libraries(
    out
    sfml-graphics
    sfml-window
)
target_compile_options(out PUBLIC -Werror=non-virtual-dtor)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_options(out PUBLIC -fsanitize=undefined,address)
    target_link_options(out PUBLIC -fsanitize=undefined,address)

else()
    target_compile_options(out PUBLIC -O3)
endif()
