/* 
Author: James Springer & Jackson Crandell
Class: ECE 4122
Last Date Modified: 12/07/21 
 
Description: Main program for fractal visualization via shaders and OpenMP
             Uses SFML for window handling and OpenGL for graphics
*/

#include <iostream>

#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "Shader.h"
#include "WindowHandler.hpp"
#include "Mandelbrot/mandelbrot_omp.h"

#define WINDOW_X 600 // starting window dimensions
#define WINDOW_Y 600


int main()
{
    sf::Window window(sf::VideoMode(WINDOW_X, WINDOW_Y), "Fractal Visualization", sf::Style::Default, sf::ContextSettings(24, 0U, 0U, 4, 3));
    window.setVerticalSyncEnabled(true);
    window.setActive(true);

    GLenum glewErr = glewInit();
    if (glewErr != GLEW_OK)
    {
        std::cerr << "GLEW initialization failed with error code: " << glewErr << std::endl;
        std::cout << "Exiting..." << std::endl;
        return EXIT_FAILURE;
    }

    // Init OpenGL Structures
    const float vertices[] =
    {
        -1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f
    };

    const unsigned int indices[] =
    {  // two triangles across the screen
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
    WindowState windowState(program_id, window.getSize().x, window.getSize().y);
    while (windowState.windowActive)
    {
        sf::Event event;

        // mode select
        FractalMode mode;
        bool active = false;
        while (!active)
        {
            while(window.pollEvent(event))
            {
                mode = windowState.modeSelect(event);
                if (mode != FractalMode::NONE || !windowState.windowActive)
                {
                    active = true;
                    break;
                }
            }
        }

        // init structures
        Shader vertexShader(program_id);
        Shader fragmentShader(program_id);
        switch (mode)
        {
            case FractalMode::SHADER_MANDELBROT:
                fragmentShader.init("shaders/mandelbrot.frag", ShaderType::Fragment);
                break;
            case FractalMode::SHADER_JULIA:
                fragmentShader.init("shaders/julia.frag", ShaderType::Fragment);
                break;
            default:
                break;
        }

        if (mode == FractalMode::SHADER_MANDELBROT || mode == FractalMode::SHADER_JULIA)
        {
            vertexShader.init("shaders/shader.vert", ShaderType::Vertex);
            if (!(vertexShader.isValid() && fragmentShader.isValid()))
            {
                std::cerr << "Shader initialization failed, exiting..." << std::endl;
                return EXIT_FAILURE;  // exit if either shader failed during initialization
            }

            // Attempt to link program
            bool success = Shader::linkShaders(program_id);
            if (!success)
            {
                std::cerr << "Failed to link shaders, exiting..." << std::endl;
                return EXIT_FAILURE;  // exit if failed to link shaders with program
            }

            windowState.shadersInit = true;
            windowState.updateFrameUniforms();
            windowState.updateWindowSizeUniforms();
            if (mode == FractalMode::OPENMP_MANDELBROT)
            {
                vertexShader.deleteShader();
                fragmentShader.deleteShader();
            }
        }

        windowState.fractalView = true;
        while (windowState.fractalView)
        {
            while (window.pollEvent(event))
            {
                windowState.handleEvent(event);
            }

            switch (mode)
            {
                case FractalMode::SHADER_MANDELBROT:
                case FractalMode::SHADER_JULIA:
                    glClearColor(0.2f, 0.0f, 0.2f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // clear buffers
                    glBindVertexArray(VAO);
                    glUseProgram(program_id);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                    break;
                case FractalMode::OPENMP_MANDELBROT:
                    omp::display(windowState.window_x, windowState.window_y, windowState.zoom, windowState.frame_x, windowState.frame_y);
                    break;
                case FractalMode::NONE:
                    windowState.fractalView = false;
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
