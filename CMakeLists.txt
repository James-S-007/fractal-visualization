cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(Fractal_Visualization)

add_executable(Fractal_Visualization main.cpp)

find_package(SFML 2.5 COMPONENTS window system REQUIRED)
find_package(OpenGL 3.3 REQUIRED COMPONENTS OpenGL)
find_package(GLEW REQUIRED)

add_dependencies(Fractal_Visualization OpenGL::OpenGL)

add_library(Shader STATIC Shader.cpp)
target_link_libraries(Shader GLEW)

target_link_libraries(Fractal_Visualization Shader sfml-system sfml-window OpenGL::OpenGL GLEW)

# include(GNUInstallDirs)
