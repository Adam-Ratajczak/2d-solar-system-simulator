cmake_minimum_required(VERSION 3.19)
project(out)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(SFML 2.5.1 COMPONENTS graphics audio REQUIRED)
find_package(PythonLibs 3.5 REQUIRED)

add_executable(
    out

    src/CanvasMode.cpp
    src/ConfigLoader.cpp
    src/EssaCreateObject.cpp
    src/EssaGUI.cpp
    src/EssaSettings.cpp
    src/FocusedObjectGUI.cpp
    src/History.cpp
    src/Object.cpp
    src/ObjectHistory.cpp
    src/SimulationView.cpp
    src/SolarSystem.cpp
    src/Trail.cpp
    src/World.cpp
    src/main.cpp

    src/gfx/RoundedEdgeRectangleShape.cpp

    src/glwrapper/Helpers.cpp
    src/glwrapper/Sphere.cpp
    src/glwrapper/Triangle.cpp
    src/glwrapper/VertexBuffer.cpp

    src/gui/Application.cpp
    src/gui/ArrowButton.cpp
    src/gui/Button.cpp
    src/gui/ColorPicker.cpp
    src/gui/Console.cpp
    src/gui/Container.cpp
    src/gui/Datebox.cpp
    src/gui/Frame.cpp
    src/gui/ImageButton.cpp
    src/gui/PythonREPL.cpp
    src/gui/SettingsMenu.cpp
    src/gui/SelectWidget.cpp
    src/gui/Slider.cpp
    src/gui/TabWidget.cpp
    src/gui/Textbox.cpp
    src/gui/TextButton.cpp
    src/gui/Textfield.cpp
    src/gui/ValueSlider.cpp
    src/gui/Widget.cpp

    src/math/Plane.cpp
    src/math/Ray.cpp
    src/math/Transform.cpp

    src/pyssa/Environment.cpp
    src/pyssa/Object.cpp

    src/util/SimulationClock.cpp
)

target_link_libraries(
    out
    sfml-graphics
    sfml-window
)
target_compile_options(out PUBLIC -Werror=non-virtual-dtor)
target_link_options(out PUBLIC -lGLEW -lGLU -lGL)

target_include_directories(out PRIVATE ${PYTHON_INCLUDE_DIRS})
target_link_libraries(out ${PYTHON_LIBRARIES})

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    target_compile_options(out PUBLIC -fsanitize=undefined,address)
    target_link_options(out PUBLIC -fsanitize=undefined,address)

else()
    target_compile_options(out PUBLIC -O3)
endif()
