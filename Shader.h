#pragma once

#include <string>

#include <GL/glew.h>  // includes typedefs for OpenGL

    enum class ShaderType {
        Vertex,
        Fragment
    };

    class Shader {
        public:
            // -methods- //
            Shader(const std::string file_path, unsigned int gl_program_id, ShaderType gl_shader_type);
            ~Shader();
            bool is_valid() const { return valid; }
            static bool link_shaders(unsigned int program_id);
        private:
            // -members- //
            const unsigned int program_id;
            ShaderType shader_type;
            GLuint shader_id;
            bool valid;

            // -methods- //
            std::string get_shader_from_file(const std::string file_path);
            bool compile_shader(const std::string& shader_code);
    };
    
