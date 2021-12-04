cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(Fractal_Visualization)

add_executable(Fractal_Visualization ${PROJECT_SOURCE_DIR}/main.cpp)

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)
find_package(OpenGL 3.3 REQUIRED COMPONENTS OpenGL)
find_package(GLEW REQUIRED)

add_dependencies(Fractal_Visualization OpenGL::OpenGL)

add_library(Shader STATIC ${PROJECT_SOURCE_DIR}/Shader.cpp)

target_link_libraries(Fractal_Visualization Shader sfml-graphics OpenGL::OpenGL GLEW)

file(COPY ${PROJECT_SOURCE_DIR}/shaders/shader.vert DESTINATION ${PROJECT_BINARY_DIR}/shaders)  # copy shaders to build directory
file(COPY ${PROJECT_SOURCE_DIR}/shaders/shader.frag DESTINATION ${PROJECT_BINARY_DIR}/shaders)
