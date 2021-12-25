#include "auxiliary_algorithms.h"
#include <fstream>
#include <iostream>

std::string readFileIntoString(const std::string& path) {
    std::ifstream inputFile(path);
    inputFile.seekg(3, std::ios::beg); // пропуск служебных символов

    if (!inputFile.is_open()) {
        std::cerr << "Could not open the file - '" << path << "'" << std::endl;
        exit(EXIT_FAILURE);
    }
    return std::string((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
}