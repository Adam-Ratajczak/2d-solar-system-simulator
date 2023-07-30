cmake_minimum_required(VERSION 3.19)
project(essa)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

option(ENABLE_PYSSA "Enable PySSA" 1)

if(ENABLE_PYSSA)
    find_package(PythonLibs 3.5 REQUIRED)
endif()

include(FetchContent)
FetchContent_Declare(Essa
    GIT_REPOSITORY https://github.com/essa-software/EssaGUI.git
    GIT_TAG cab7ff93a5a439b8ba0c8b463d7ce19db67ca29d
    OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(Essa)

if(ENABLE_PYSSA)
    set(PYSSA_SOURCES 
        src/essagui/PythonREPL.cpp
        src/pyssa/Environment.cpp
        src/pyssa/Object.cpp
    )
else()
    set(PYSSA_SOURCES)
endif()

essa_executable(essa
    LIBS
    Essa::GUI
    Essa::Engine-3D

    SOURCES
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
essa_resources(essa assets)

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
