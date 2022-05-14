cmake_minimum_required(VERSION 3.19)
project(out)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(SFML 2.5.1 COMPONENTS graphics audio REQUIRED)
find_package(PythonLibs 3.5 REQUIRED)

add_executable(
    out

    src/ConfigLoader.cpp
    src/History.cpp
    src/Object.cpp
    src/ObjectHistory.cpp
    src/SimulationView.cpp
    src/SolarSystem.cpp
    src/Trail.cpp
    src/World.cpp
    src/main.cpp
    
    src/essagui/EssaCanvasMode.cpp
    src/essagui/EssaCreateObject.cpp
    src/essagui/EssaGUI.cpp
    src/essagui/EssaSettings.cpp
    src/essagui/FocusedObjectGUI.cpp
    src/essagui/PythonREPL.cpp

    src/gfx/ClipViewScope.cpp
    src/gfx/RoundedEdgeRectangleShape.cpp

    src/glwrapper/Helpers.cpp
    src/glwrapper/Sphere.cpp

    src/math/Plane.cpp
    src/math/Ray.cpp
    src/math/Transform.cpp

    src/pyssa/Environment.cpp
    src/pyssa/Object.cpp

    src/util/SimulationClock.cpp
    src/util/UnitDisplay.cpp
)

target_compile_options(out PUBLIC -fdiagnostics-color=always)

function(setup_target targetname)
    target_compile_options(${targetname} PUBLIC -Werror=non-virtual-dtor)

    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        target_compile_options(${targetname} PUBLIC -fsanitize=undefined,address)
        target_link_options(${targetname} PUBLIC -fsanitize=undefined,address)
    else()
        target_compile_options(${targetname} PUBLIC -O3)
    endif()
endfunction()
setup_target(out)

target_link_libraries(out essa-gui GLEW GLU GL)
target_link_libraries(out ${PYTHON_LIBRARIES})
target_include_directories(out PRIVATE ${PYTHON_INCLUDE_DIRS})

add_subdirectory(src/gui)
