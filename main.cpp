#include <iostream>

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "Shader.h"
#include "WindowHandler.hpp"
#include "Mandelbrot/mandelbrot_omp.h"

#define WINDOW_X 600 // starting window dimensions
#define WINDOW_Y 600


int main() {
    sf::Window window(sf::VideoMode(WINDOW_X, WINDOW_Y), "Fractal Visualization", sf::Style::Default, sf::ContextSettings(24, 0U, 0U, 4, 3));
    window.setVerticalSyncEnabled(true);
    window.setActive(true);

    GLenum glew_err = glewInit();
    if (glew_err != GLEW_OK) {
        std::cerr << "GLEW initialization failed with error code: " << glew_err << std::endl;
        std::cout << "Exiting..." << std::endl;
        return EXIT_FAILURE;
    }

    // Init OpenGL Structures
    const float vertices[] = {
        -1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f
    };

    const unsigned int indices[] = {  // two triangles across the screen
        0, 1, 2,
        2, 3, 0
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  // TODO(James): do we want static, dynamic, etc.?
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    // // Create OpenGL program and init shaders
    GLuint program_id = glCreateProgram();

    // Define uniforms
    WindowState window_state(program_id, window.getSize().x, window.getSize().y);
    while (window_state.window_active) {
        sf::Event event;

        // mode select
        FractalMode mode;
        bool active = false;
        while (!active) {
            while(window.pollEvent(event)) {
                mode = window_state.mode_select(event);
                if (mode != FractalMode::NONE || !window_state.window_active) {
                    active = true;
                    break;
                }
            }
        }

        // init structures
        Shader vertex_shader(program_id);
        Shader fragment_shader(program_id);
        switch (mode) {
            case FractalMode::SHADER_MANDELBROT:
                fragment_shader.init("shaders/mandelbrot.frag", ShaderType::Fragment);
                break;
            case FractalMode::SHADER_JULIA:
                fragment_shader.init("shaders/julia.frag", ShaderType::Fragment);
                break;
            default:
                break;
        }

        if (mode == FractalMode::SHADER_MANDELBROT || mode == FractalMode::SHADER_JULIA) {
            vertex_shader.init("shaders/shader.vert", ShaderType::Vertex);
            if (!(vertex_shader.is_valid() && fragment_shader.is_valid())) {
                std::cerr << "Shader initialization failed, exiting..." << std::endl;
                return EXIT_FAILURE;  // exit if either shader failed during initialization
            }

            // Attempt to link program
            bool success = Shader::link_shaders(program_id);
            if (!success) {
                std::cerr << "Failed to link shaders, exiting..." << std::endl;
                return EXIT_FAILURE;  // exit if failed to link shaders with program
            }

            window_state.shaders_init = true;
            window_state.update_frame_uniforms();
            window_state.update_window_size_uniforms();
            if (mode == FractalMode::OPENMP_MANDELBROT) {
                vertex_shader.deleteShader();
                fragment_shader.deleteShader();
            }
        }

        window_state.fractal_view = true;
        while (window_state.fractal_view) {
            while (window.pollEvent(event)) {
                window_state.handle_event(event);
            }

            switch (mode) {
                case FractalMode::SHADER_MANDELBROT:
                case FractalMode::SHADER_JULIA: {
                    glClearColor(0.2f, 0.0f, 0.2f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // clear buffers
                    glBindVertexArray(VAO);
                    glUseProgram(program_id);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                    break;
                }
                case FractalMode::OPENMP_MANDELBROT:
                    omp::display(window_state.window_x, window_state.window_y, window_state.zoom, window_state.frame_x, window_state.frame_y);
                    break;
            }
            
            window.display();
        }
    }

    // Release resources
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VAO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(program_id);
    program_id = 0;

    return EXIT_SUCCESS;
}
