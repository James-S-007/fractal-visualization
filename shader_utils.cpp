#include <fstream>
#include <iostream>
#include <sstream>

#include "shader_utils.h"

namespace shader_utils {

    // Input: file path to shader
    // Returns:
        // Empty string if unable to read from file
        // std::string containing shader
    std::string get_shader_from_file(const std::string file_path) {
        std::stringstream shader;
        std::ifstream file(file_path, std::ios::in);

        if (!file.is_open()) {
            std::cerr << "Error reading from file: " << file_path << std::endl;
            return "";
        }

        shader << file.rdbuf();
        return shader.str();
    }

}  // namespace shader_utils
