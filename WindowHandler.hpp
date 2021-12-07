#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>


struct WindowState {
    public: 
        bool window_active;
        float zoom;
        float frame_x;
        float frame_y;
        int mouse_x;
        int mouse_y;
        bool panning;
        float max_zoom;
        int window_x;  // dim in pixels
        int window_y;  // dim in pixels
    private:
        const GLuint program_id;

    public:
        WindowState(GLuint program_id, int window_x, int window_y, float max_zoom = .000001f) : program_id(program_id), \
                                window_x(window_x), window_y(window_y), window_active(true), zoom(1.f), frame_x(0.f), \
                                frame_y(0.f), mouse_x(0), mouse_y(0), panning(false), max_zoom(max_zoom) {
            this->update_frame_uniforms();
            this->update_window_size_uniforms();
        }

        bool handle_event(const sf::Event& event) {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)) {
                window_active = false;  // end program
                return true;
            } else if (event.type == sf::Event::Resized) {
                window_x = event.size.width;
                window_y = event.size.height;
                glViewport(0, 0, window_x, window_y);  // adjust window size
                this->update_window_size_uniforms();
                return true;
            } else if (event.type == sf::Event::MouseWheelScrolled) {
                zoom = zoom * (1 - .07 * event.mouseWheelScroll.delta);
                zoom = (zoom > 1.0f) ? 1.0f : zoom;
                zoom = (zoom < .00001f) ? .00001f: zoom;
                this->update_frame_uniforms();
                return true;
            } else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Left) {
                panning = true;
                mouse_x = event.mouseButton.x;
                mouse_y = event.mouseButton.y;
                return true;
            } else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Left) {
                panning = false;
                return true;
            } else if (event.type == sf::Event::MouseMoved && panning) {
                frame_x += (mouse_x - event.mouseMove.x) / float(window_x) * zoom;  // TODO(James): make depenendent on window size
                frame_y += (event.mouseMove.y - mouse_y) / float(window_y) * zoom;
                frame_x = (frame_x > 1.0f) ? 1.0f : frame_x;
                frame_x = (frame_x < -1.0f) ? -1.0f : frame_x;
                frame_y = (frame_y > 1.0f) ? 1.0f : frame_y;
                frame_y = (frame_y < -1.0f) ? -1.0f : frame_y;
                mouse_x = event.mouseMove.x;
                mouse_y = event.mouseMove.y;
                this->update_frame_uniforms();
                return true;
            } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                zoom = 1; // reset zoom and frame
                frame_x = 0;
                frame_y = 0;
                this->update_frame_uniforms();
                return true;
            }

            return false;
        }

        void update_frame_uniforms() {
            glUseProgram(program_id);
            glUniform1f(glGetUniformLocation(program_id, "zoom"), zoom);
            glUniform1f(glGetUniformLocation(program_id, "frame_x"), frame_x);
            glUniform1f(glGetUniformLocation(program_id, "frame_y"), frame_y);
        }

        void update_window_size_uniforms() {
            glUseProgram(program_id);
            // don't allow horizontal or vertical stretching, get min of two arguments
            int min_dim = (window_x < window_y) ? window_x : window_y;
            glUniform1i(glGetUniformLocation(program_id, "width"), min_dim);
            glUniform1i(glGetUniformLocation(program_id, "height"), min_dim);
        }
};
