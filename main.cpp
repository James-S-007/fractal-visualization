#include <iostream>

#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

#include "Shader.h"


int main() {
    sf::Window window(sf::VideoMode(800, 800), "Fractal Visualization", sf::Style::Default, sf::ContextSettings(24, 0U, 0U, 4, 3));
    window.setVerticalSyncEnabled(true);
    window.setActive(true);

    // Init OpenGL Structures
    const float vertices[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f   
    };
    
    const unsigned int indices[] = {
        0, 1, 2,
        0, 3, 1
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

    // Create OpenGL program and init shaders
    GLuint program_id = glCreateProgram();
    Shader vertex_shader("shaders/shader.vert", program_id, ShaderType::Vertex);
    Shader fragment_shader("shaders/shader.frag", program_id, ShaderType::Fragment);
    if (!(vertex_shader.is_valid() && fragment_shader.is_valid())) {
        std::cerr << "Shader initialization failed, exiting..." << std::endl;
        return EXIT_FAILURE;  // exit if either shader failed during initialization
    }

    bool active = true;
    while (active) {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                active = false;  // end program
            } else if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);  // adjust window size
            }
        }

        glClearColor(0.2f, 0.0f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // clear buffers
        glViewport(0, 0, window.getSize().x, window.getSize().y);  // TODO(James): is this necessary?

        glUseProgram(program_id);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // end the current frame (internally swaps the front and back buffers)
        window.display();
    }

    // release resources
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VAO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(program_id);
    program_id = 0;

    return 0;
}

