#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace utils {

std::string read_file_contents(const std::string& path) {

    std::ifstream file(path);
    if(!file.is_open()) {
        std::cerr << "Error reading file: " << path << std::endl;
        std::exit(1);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

}