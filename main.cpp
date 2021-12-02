#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

#include <iostream>

// create vertex shader (string)
const std::string vertex_shader = "";

// create fragment shader
const std::string fragment_shader = "";



int main() {
    sf::Window window(sf::VideoMode(800, 800), "OpenGL", sf::Style::Default, sf::ContextSettings(24, 0U, 0U, 4, 3));
    window.setVerticalSyncEnabled(true);

    window.setActive(true);

    // load resources, initialize the OpenGL states, ...

    // run the main loop
    bool running = true;
    while (running)
    {
        // handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                // end the program
                running = false;
            }
            else if (event.type == sf::Event::Resized)
            {
                // adjust the viewport when the window is resized
                glViewport(0, 0, event.size.width, event.size.height);
            }
        }

        // clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw...

        // end the current frame (internally swaps the front and back buffers)
        window.display();
    }

    // release resources...

    return 0;
}

