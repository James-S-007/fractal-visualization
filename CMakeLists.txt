cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(Fractal_Visualization)

add_executable(Fractal_Visualization ${PROJECT_SOURCE_DIR}/main.cpp)
add_executable(Tetra ${PROJECT_SOURCE_DIR}/Tetra.cpp)

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)
find_package(OpenGL 3.3 REQUIRED COMPONENTS OpenGL)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

add_dependencies(Fractal_Visualization OpenGL::OpenGL)
add_dependencies(Tetra OpenGL::OpenGL)

add_library(Shader STATIC ${PROJECT_SOURCE_DIR}/Shader.cpp)
add_library(Omp STATIC ${PROJECT_SOURCE_DIR}/Mandelbrot/mandelbrot_omp.cpp)

target_link_libraries(Omp GLU)
target_link_libraries(Fractal_Visualization Shader Omp sfml-graphics OpenGL::OpenGL GLEW)
target_link_libraries(Tetra OpenGL::OpenGL GLEW ${GLUT_LIBRARY})

file(COPY ${PROJECT_SOURCE_DIR}/shaders/shader.vert DESTINATION ${PROJECT_BINARY_DIR}/shaders)  # copy shaders to build directory
file(COPY ${PROJECT_SOURCE_DIR}/shaders/mandelbrot.frag DESTINATION ${PROJECT_BINARY_DIR}/shaders)
file(COPY ${PROJECT_SOURCE_DIR}/shaders/julia.frag DESTINATION ${PROJECT_BINARY_DIR}/shaders)

