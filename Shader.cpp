/* 
Author: James Springer
Class: ECE 4122
Last Date Modified: 12/07/21 
 
Description: Wrapper class for OpenGL shaders
*/

#include <fstream>
#include <iostream>
#include <sstream>

#include "Shader.h"

Shader::Shader(unsigned int gl_program_id) : program_id(gl_program_id), shaderType(ShaderType::NONE), valid(false) {}

Shader::Shader(const std::string file_path, unsigned int gl_program_id, ShaderType gl_shaderType) : program_id(gl_program_id), shaderType(gl_shaderType), valid(false)
{
    const std::string shaderCode = getShaderFromFile(file_path);
    if (!shaderCode.empty())
    {
        bool success = compileShader(shaderCode);
        if (success)
        {
            valid = true;
        }
    }
}

Shader::~Shader()
{
    glDetachShader(program_id, shader_id);
    glDeleteShader(shader_id);
}

void Shader::init(const std::string file_path, ShaderType gl_shaderType)
{
    shaderType = gl_shaderType;  
    const std::string shaderCode = getShaderFromFile(file_path);
    if (!shaderCode.empty())
    {
        bool success = compileShader(shaderCode);
        if (success)
        {
            valid = true;
        }
    }  
}

void Shader::deleteShader()
{
    glDetachShader(program_id, shader_id);
    valid = false;
}


// Input: file path to shader
// Returns: std::string containing shader or empty string if unable to read from file
std::string Shader::getShaderFromFile(const std::string file_path)
{
    std::stringstream shader;
    std::ifstream file(file_path, std::ios::in);

    if (!file.is_open())
    {
        std::cerr << "Error reading from file: " << file_path << std::endl;
        return "";
    }

    shader << file.rdbuf();
    file.close();
    return shader.str();
}

// Input: string of GLSL shader code
// Returns: true if successfully compiled, false if error
bool Shader::compileShader(const std::string& shaderCode)
{
    switch (shaderType)
    {
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
    sources[0] = shaderCode.c_str();
    lengths[0] = shaderCode.length();

    glShaderSource(shader_id, 1, sources, lengths);
    glCompileShader(shader_id);

    // check for errors
    int success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLchar errMsg[1024] = { 0 };
        glGetShaderInfoLog(shader_id, sizeof(errMsg), nullptr, errMsg);
        std::cerr << "Failed to compile shader with following error..." << std::endl;
        std::cerr << errMsg << std::endl;
        return false;
    }

    glAttachShader(program_id, shader_id);
    return true;
}

// Input: program_id (static function because does not explicitly use shader)
// Returns: true if successfully linked shaders with program, false if error
bool Shader::linkShaders(unsigned int program_id)
{
    glLinkProgram(program_id);
    int success;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLchar errMsg[1024] = { 0 };
        glGetProgramInfoLog(program_id, sizeof(errMsg), nullptr, errMsg);
        std::cerr << "Failed to link shaders with program" << std::endl;
        std::cerr << errMsg << std::endl;
        return false;
    }

    return true;
}
