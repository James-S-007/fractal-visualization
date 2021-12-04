#include <iostream>

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "Shader.h"


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
    float zoom = 1;
    float frame_x = 0;
    float frame_y = 0;
    glUseProgram(program_id);
    glUniform1f(glGetUniformLocation(program_id, "zoom"), zoom);
    glUniform1f(glGetUniformLocation(program_id, "frame_x"), frame_x);
    glUniform1f(glGetUniformLocation(program_id, "frame_y"), frame_y);

    // Window and pan variables
    bool active = true;
    bool panning = false;  // used to enable drag pan control
    int mouse_x, mouse_y;
    while (active) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)) {
                active = false;  // end program
            } else if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);  // adjust window size
            } else if (event.type == sf::Event::MouseWheelScrolled) {
                zoom = zoom * (1 - .07 * event.mouseWheelScroll.delta);
                zoom = (zoom > 1.0f) ? 1.0f : zoom;
                zoom = (zoom < .00001f) ? .00001f: zoom;
                glUniform1f(glGetUniformLocation(program_id, "zoom"), zoom);
            } else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Left) {
                panning = true;
                mouse_x = event.mouseButton.x;
                mouse_y = event.mouseButton.y;
            } else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Left) {
                panning = false;
            } else if (event.type == sf::Event::MouseMoved && panning) {
                frame_x += (mouse_x - event.mouseMove.x) / 600.0f * zoom;  // TODO(James): make depenendent on window size
                frame_y += (event.mouseMove.y - mouse_y) / 600.0f * zoom;
                frame_x = (frame_x > 1.0f) ? 1.0f : frame_x;
                frame_x = (frame_x < -1.0f) ? -1.0f : frame_x;
                frame_y = (frame_y > 1.0f) ? 1.0f : frame_y;
                frame_y = (frame_y < -1.0f) ? -1.0f : frame_y;
                mouse_x = event.mouseMove.x;
                mouse_y = event.mouseMove.y;
                glUniform1f(glGetUniformLocation(program_id, "frame_x"), frame_x);
                glUniform1f(glGetUniformLocation(program_id, "frame_y"), frame_y);
            } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                zoom = 1; // reset zoom and frame
                frame_x = 0;
                frame_y = 0;
                glUniform1f(glGetUniformLocation(program_id, "zoom"), zoom);
                glUniform1f(glGetUniformLocation(program_id, "frame_x"), frame_x);
                glUniform1f(glGetUniformLocation(program_id, "frame_y"), frame_y);
            }
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

    return 0;
}
