cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(Fractal_Visualization)

find_package(SFML 2.5 COMPONENTS window system REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(Fractal_Visualization main.cpp)

add_library(shader_utils STATIC shader_utils.cpp)

target_link_libraries(Fractal_Visualization shader_utils sfml-system sfml-window GL)

include(GNUInstallDirs)
