cmake_minimum_required(VERSION 3.19)
project(essa)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

option(ENABLE_PYSSA "Enable PySSA" 1)

if(ENABLE_PYSSA)
    find_package(PythonLibs 3.5 REQUIRED)
endif()


if(USE_LOCAL_ESSA_FRAMEWORK)
    find_package(Essa)
else()
    include(FetchContent)
    FetchContent_Declare(Essa
        GIT_REPOSITORY https://github.com/essa-software/EssaGUI.git
        GIT_TAG 4500cf811c0b9f31ee7bb97f81e0583af454f3c2
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(Essa)
endif()

if(ENABLE_PYSSA)
    set(PYSSA_SOURCES 
        src/essagui/PythonREPL.cpp
        src/pyssa/Environment.cpp
        src/pyssa/Object.cpp
    )
else()
    set(PYSSA_SOURCES)
endif()

add_executable(
    essa

    src/ConfigLoader.cpp
    src/History.cpp
    src/Object.cpp
    src/ObjectHistory.cpp
    src/SimulationView.cpp
    src/Trail.cpp
    src/World.cpp
    src/main.cpp
    
    src/essagui/EssaCreateObject.cpp
    src/essagui/EssaGUI.cpp
    src/essagui/EssaSettings.cpp
    src/essagui/EssaSplash.cpp
    src/essagui/FocusedObjectGUI.cpp
    src/essagui/SimulationInfo.cpp

    src/glwrapper/Sphere.cpp

    ${PYSSA_SOURCES}
)

target_compile_options(essa PUBLIC -fdiagnostics-color=always)

essautil_setup_target(essa)

target_link_libraries(essa Essa::GUI Essa::Engine-3D)
target_include_directories(essa PUBLIC ${Essa_SOURCE_DIR}) # FIXME: This should be automatic

install(TARGETS essa DESTINATION bin)

if(ENABLE_PYSSA)
    message("Enabling PySSA")
    target_link_libraries(essa ${PYTHON_LIBRARIES})
    target_include_directories(essa PRIVATE ${PYTHON_INCLUDE_DIRS})
    target_compile_definitions(essa PRIVATE ENABLE_PYSSA=1)
endif()

essautil_setup_packaging()

set(CPACK_IGNORE_FILES *.hpp;*.h)
include(CPack)
