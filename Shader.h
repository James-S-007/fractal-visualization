/* 
Author: James Springer
Class: ECE 4122
Last Date Modified: 12/07/21 
 
Description: Wrapper class for OpenGL shaders
*/

#pragma once

#include <string>

#include <GL/glew.h>  // includes typedefs for OpenGL

    enum class ShaderType
    {
        Vertex,
        Fragment,
        NONE
    };

    class Shader
    {
        public:
            // -methods- //

            // Uninitialized constructor, not valid until init is successfully called
            Shader(unsigned int gl_program_id);
            Shader(const std::string file_path, unsigned int gl_program_id, ShaderType gl_shaderType);
            ~Shader();
            bool isValid() const { return valid; }

            // Initializes shader
            void init(const std::string file_path, ShaderType gl_shaderType);  // performs necessary initializations
            
            // Detaches shader from current OpenGL instance
            void deleteShader(); 

            // Links all shaders with current program_id
            static bool linkShaders(unsigned int program_id);
        private:
            // -members- //
            const unsigned int program_id;
            ShaderType shaderType;
            GLuint shader_id;
            bool valid;

            // -methods- //

            // Loads code from file into shader string
            std::string getShaderFromFile(const std::string file_path);

            // Compile shader and check validity
            bool compileShader(const std::string& shaderCode);
    };
    
