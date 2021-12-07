#include <fstream>
#include <iostream>
#include <sstream>

#include "Shader.h"

Shader::Shader(unsigned int gl_program_id) : program_id(gl_program_id), shader_type(ShaderType::NONE), valid(false) {}

Shader::Shader(const std::string file_path, unsigned int gl_program_id, ShaderType gl_shader_type) : program_id(gl_program_id), shader_type(gl_shader_type), valid(false) {
    const std::string shader_code = get_shader_from_file(file_path);
    if (!shader_code.empty()) {
        bool success = compile_shader(shader_code);
        if (success) {
            valid = true;
        }
    }
}

Shader::~Shader() {
    glDetachShader(program_id, shader_id);
    glDeleteShader(shader_id);
}

void Shader::init(const std::string file_path, ShaderType gl_shader_type) {
    shader_type = gl_shader_type;  
    const std::string shader_code = get_shader_from_file(file_path);
    if (!shader_code.empty()) {
        bool success = compile_shader(shader_code);
        if (success) {
            valid = true;
        }
    }  
}

void Shader::deleteShader() {
    glDetachShader(program_id, shader_id);
    glDeleteShader(shader_id);
}


// Input: file path to shader
// Returns: std::string containing shader or empty string if unable to read from file
std::string Shader::get_shader_from_file(const std::string file_path) {
    std::stringstream shader;
    std::ifstream file(file_path, std::ios::in);

    if (!file.is_open()) {
        std::cerr << "Error reading from file: " << file_path << std::endl;
        return "";
    }

    shader << file.rdbuf();
    file.close();
    return shader.str();
}

// Input: string of GLSL shader code
// Returns: true if successfully compiled, false if error
bool Shader::compile_shader(const std::string& shader_code) {
    switch (shader_type) {
        case ShaderType::Vertex:
            shader_id = glCreateShader(GL_VERTEX_SHADER);
            break;
        case ShaderType::Fragment:
            shader_id = glCreateShader(GL_FRAGMENT_SHADER);
            break;
        default:
            std::cerr << "Invalid shader type!" << std::endl;
            return false;
    }

    const GLchar* sources[1];
    GLint lengths[1];
    sources[0] = shader_code.c_str();
    lengths[0] = shader_code.length();

    glShaderSource(shader_id, 1, sources, lengths);
    glCompileShader(shader_id);

    // check for errors
    int success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLchar error_msg[1024] = { 0 };
        glGetShaderInfoLog(shader_id, sizeof(error_msg), nullptr, error_msg);
        std::cerr << "Failed to compile shader with following error..." << std::endl;
        std::cerr << error_msg << std::endl;
        return false;
    }

    glAttachShader(program_id, shader_id);
    return true;
}

// Input: program_id (static function because does not explicitly use shader)
// Returns: true if successfully linked shaders with program, false if error
bool Shader::link_shaders(unsigned int program_id) {
    glLinkProgram(program_id);
    int success;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        GLchar error_msg[1024] = { 0 };
        glGetProgramInfoLog(program_id, sizeof(error_msg), nullptr, error_msg);
        std::cerr << "Failed to link shaders with program" << std::endl;
        std::cerr << error_msg << std::endl;
        return false;
    }

    return true;
}
