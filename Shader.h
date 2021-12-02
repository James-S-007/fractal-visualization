#pragma once

#include <string>

#include <GL/glew.h>  // includes typedefs for OpenGL

    enum class ShaderType {
        Vertex,
        Fragment
    };

    class Shader {
        public:
            Shader(const std::string file_path, unsigned int gl_program_id, ShaderType gl_shader_type);
            ~Shader();
        private:
            // -members- //
            const unsigned int program_id;
            ShaderType shader_type;
            GLuint shader_id;

            // -methods- //
            std::string get_shader_from_file(const std::string file_path);
            bool compile_shader(const std::string& shader_code);
    };
