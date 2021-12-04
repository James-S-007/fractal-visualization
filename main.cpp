#include <iostream>

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "Shader.h"
#include "WindowHandler.hpp"


int main() {
    sf::Window window(sf::VideoMode(600, 600), "Fractal Visualization", sf::Style::Default, sf::ContextSettings(24, 0U, 0U, 4, 3));
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

    // Create OpenGL program and init shaders
    GLuint program_id = glCreateProgram();
    Shader vertex_shader("shaders/shader.vert", program_id, ShaderType::Vertex);
    Shader fragment_shader("shaders/shader.frag", program_id, ShaderType::Fragment);
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

    // Define uniforms
    WindowState window_state(program_id);
    window_state.update_uniforms();

    while (window_state.window_active) {
        sf::Event event;
        while (window.pollEvent(event)) {
            window_state.handle_event(event);
        }

        glClearColor(0.2f, 0.0f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // clear buffers
        // glViewport(0, 0, window.getSize().x, window.getSize().y);  // TODO(James): is this necessary?

        glBindVertexArray(VAO);
        glUseProgram(program_id);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        window.display();
    }

    // Release resources
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VAO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(program_id);
    program_id = 0;

    return EXIT_SUCCESS;
}
