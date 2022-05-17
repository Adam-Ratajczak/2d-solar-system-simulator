# ESSA build instructions

This guide will show you how to build and run ESSA from the source.

## Install required dependencies

* `cmake` - as a build system
* Some "lower-level" build system, we use `ninja` but `make` or Visual Studio should work
* `SFML` - for GUI
* `GLEW` - currently for nothing but is linked
* `PythonLibs` - for PySSA

## Setup CMake

From the project root directory:
```sh
mkdir build
cd build
cmake .. -GNinja
```

(Replace `-GNinja` with your generator)

## Build the project

From the `build` directory:
```sh
ninja
```

## Run

From the `build` directory (This is required because otherwise the app won't see resources, see #10):
```sh
./out
```
